#include "json.hpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <format>
#include <fstream>
#include <ctime>
#include <clocale>
#include "SongBookUtils.h"
#include <sqlite3.h>
#include <filesystem>
#include "SongDatabase.h"

static std::vector<std::map<std::string, std::string>> sql_contents;

SongDatabase::SongDatabase()  {

    std::string db_file_path = SongBookUtils::getInstance()->getConfigItem("paths/db_file_path");

    if (db_file_path.empty())  {
      db_file_path = "db.sql";
    }

    int exit = sqlite3_open(db_file_path.c_str(), &DB);

    // Set Czech locale for czech sort
    if (!std::setlocale(LC_COLLATE, "cs_CZ.UTF-8")) {
        std::cerr << "ERROR: Failed to set Czech locale!" << std::endl;
    } else {
      sqlite3_create_collation(DB, "CZECH", SQLITE_UTF8, nullptr, SongDatabase::czech_collation);
    }

    if (exit) {
        std::cout << "Couldn't open SQL database `" <<  db_file_path << "`!" << std::endl;
    }
    else {
      std::ostringstream sql;
      
      // table SONGS contains all the songs with title, artist and lyrics (in bard format)
      sql << "CREATE TABLE SONGS(";
      sql << "ID INT PRIMARY KEY NOT NULL,\n";
      sql << "TITLE TEXT NOT NULL,\n";
      sql << "ARTIST TEXT NOT NULL,\n";
      sql << "LYRICS TEXT);\n\n";
      
      // table COLLECTIONS contains all collections present in database
      sql << "CREATE TABLE COLLECTIONS(";
      sql << "ID INT PRIMARY KEY,\n";
      sql << "NAME TEXT UNIQUE\n";
      sql << ");";

      // this aux. table maps individual songs to collections
      sql << "CREATE TABLE COLLECTION_MAP(";
      sql << "SONG_ID INT,\n";
      sql << "COLLECTION_ID INT,\n";
      sql << "PRIMARY KEY (SONG_ID, COLLECTION_ID),\n";
      sql << "FOREIGN KEY (SONG_ID) REFERENCES SONGS(ID),\n";
      sql << "FOREIGN KEY (COLLECTION_ID) REFERENCES COLLECTIONS(ID));";
      
      char* messaggeError;
      exit = sqlite3_exec(DB, sql.str().c_str(), NULL, 0, &messaggeError);
      
      if (exit == SQLITE_OK) {
          std::cout << "Could not find SQL database file, created empty one ..." << std::endl;
          sqlite3_free(messaggeError);
      }
    }
}

/** Aux. SQL callback function that returns all contents from sql table as vector */
static int sql_cb(void* data, int argc, char** argv, char** azColName)
{
    std::map<std::string, std::string> m = std::map<std::string, std::string>();

    for (int i = 0; i < argc; i++) {
        m[azColName[i]] =  argv[i] ? argv[i] : "NULL";
    }

    sql_contents.push_back(m);

    return 0;
}

int SongDatabase::czech_collation(void*,
                                  int len1, const void* str1,
                                  int len2, const void* str2) 
{
    // Copy strings into null-terminated buffers
    std::string a((const char*)str1, len1);
    std::string b((const char*)str2, len2);
    return strcoll(a.c_str(), b.c_str());
}

int SongDatabase::SendQuery(std::string query)  {
    char* messageError;
    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, 0, &messageError);

    std::cout << "exit code: " << exit << std::endl;
    if (exit)  {
      std::cout << "Message: " << messageError << std::endl;
    }

    sql_contents.clear();

    return 0;
}

int SongDatabase::changeOrder(std::string order)  {
    // Convert to uppercase
    for (int i = 0; i < order.size(); i++)  {
      order[i] = std::toupper(order[i]);
    }

    // Check if property is present...
    bool isProp = false;
    for (const auto& prop : song_properties)  {
      if (order == prop)  {
        isProp = true;
      }
    }

    // If property is not present, return...
    if (!isProp)  {
      return 1;
    }

    this->order = order;
    return 0;
}



SongDatabase::~SongDatabase()  {
    if (!this) {
      delete this;
    }
}

int SongDatabase::compare(std::string firstString, std::string secondString)  {

    std::string str1 = SongBookUtils::getInstance()->convert_to_ascii(firstString);
    std::string str2 = SongBookUtils::getInstance()->convert_to_ascii(secondString);


    for (int i = 0; i < str1.size(); i++)  {
      
      // tranform 'ch' character to 'h'+1 (i)
      if (str1[i] == 'c' && i < str1.size()-1)  {
        if (str1[i+1] == 'h')  {
          str1[i] = 'h' + 1;
        }
      }
      if (str2[i] == 'c' && i < str2.size()-1)  {
        if (str2[i+1] == 'h')  {
          str2[i] = 'h' + 1;
        }
      }

      if (str1[i] < str2[i])  {
        return -1;
      }
      else if (str1[i] > str2[i])  {
        return 1;
      }
    }

    return 0;
}

nlohmann::json SongDatabase::getCollections()
{
  std::string pattern = SongBookUtils::getConfigItem("workspace/filter_pattern", "");

  if (pattern == "")  {
    return getSqlJson("SELECT * FROM COLLECTIONS");
  }
  else  {
    return getSqlJson(std::format("SELECT * FROM COLLECTIONS WHERE NAME LIKE '%{}%'", pattern));
  }
}


int SongDatabase::addSong(nlohmann::json json_string, bool override)  {

    // obtain unique ID
    nlohmann::json sql_json = getSqlJson("SELECT * FROM SONGS ORDER BY ID DESC LIMIT 1;");
    
    std::string available_id = (sql_json.empty()) ? "0" : sql_json[0]["ID"].get<std::string>();

    try
    {
      available_id = std::to_string(std::stoi(available_id) + 1);
    }
    catch(const std::exception& e)
    {
      std::cout << "Error while trying to get incremental ID for new item!" << std::endl;
      return 2;
    }
    
    nlohmann::json j = json_string;

    std::string query;

    if (j.contains("ID") && songExists(std::stoi(j["ID"].get<std::string>())))  {
      nlohmann::json song = getSong(std::stoi(j["ID"].get<std::string>()));
      if (override)  {
        query = "UPDATE SONGS SET ";
        int i = 0;
        for (const auto& p : song_properties) {
          query += (i++ ? ",\n" : "\n") + (j.contains(p) ? (p + " = \'" + SongBookUtils::txt2sql(j[p].get<std::string>()) + "\'") : (p + " = NULL"));
        
        }
        query += "\n WHERE ID=" + song["ID"].get<std::string>() + ";";
      }
      else  {
      // song is already present and override is false, hence don't do anything...
      return 1;
      }
    }
    else {
      query = "INSERT INTO SONGS VALUES(" + available_id;
      for (const auto& p : song_properties) {
        query += (j.contains(p)) ? (", '" + SongBookUtils::txt2sql(j[p].get<std::string>()) + "'") : (", NULL");
      }
      query += ");";
    }

    std::cerr << query.c_str() << std::endl;

    char* messageError;
    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, nullptr, &messageError);

    if (exit)  {
      std::cerr << messageError << std::endl;
    }

    return exit;
}

/** Get json representation of sql contents
 *  Returns json or null
 */
nlohmann::json SongDatabase::getSqlJson(std::string query)  {

    if (query == "") {
        query = "SELECT * FROM SONGS ORDER BY " + order + " COLLATE CZECH";
    }

    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, nullptr, nullptr);

    nlohmann::json data = nlohmann::json();

    if (exit)  {
        return data;
    }
    
    int i = 0;
    for (const auto& content : sql_contents)  {
        nlohmann::json item  = nlohmann::json();

        item["NO"] = std::to_string(i);

        for (const auto& [k, v] : content)  {
            item[k] = v;
        }

        data[i++] = item;
    }

    sql_contents.clear();

    return data;
}

nlohmann::json SongDatabase::getJson()
{
  std::string pattern = SongBookUtils::getConfigItem("workspace/filter_pattern", "");
  std::string collection_id = SongBookUtils::getConfigItem("workspace/filter_collection", "-1");

  if (collection_id == "-1" && pattern == "")  {
    return getSqlJson("SELECT * FROM SONGS");
  }
  else if (collection_id == "-1" && pattern != "")  {
    return getSqlJson(std::format("SELECT * FROM SONGS WHERE TITLE LIKE '%{}%'", pattern));
  }
  else if (collection_id != "-1" && pattern == "")  {
    SongBookUtils::printError("dsadsa");
    return getSqlJson(std::format("SELECT s.* FROM SONGS s JOIN COLLECTION_MAP cm ON s.id = cm.song_id WHERE cm.collection_id = {} ;", collection_id));
  }
  else {
    return getSqlJson(std::format("SELECT s.* FROM SONGS s JOIN COLLECTION_MAP cm ON s.id = cm.song_id WHERE cm.collection_id = {} AND TITLE LIKE '%{}%';", collection_id, pattern));
  }
}

nlohmann::json SongDatabase::getSong(int id)
{   
    std::string query = "SELECT * FROM SONGS WHERE ID=" + std::to_string(id) + ";";
    nlohmann::json sql_json = getSqlJson(query);

    if (sql_json.size() != 1)  {
      // TODO fatal error! There cannot be two items with same ID!
    }

    return sql_json[0];
}

nlohmann::json SongDatabase::getSong(std::string title, std::string artist)
{   
    std::string query = "SELECT * FROM SONGS WHERE TITLE='" + title + "';";
    nlohmann::json sql_json = getSqlJson(query);

    return sql_json[0];
}

nlohmann::json SongDatabase::findSong(std::string pattern)
{
  std::string query = "SELECT * FROM SONGS WHERE TITLE LIKE '%" + pattern + "%';";
  return getSqlJson(query);
}

bool SongDatabase::songExists(std::string title, std::string artist)
{
  return (getSong(title, artist).size() > 0);
}

bool SongDatabase::collectionExists(std::string name)
{
  nlohmann::json col = getSqlJson(std::format("SELECT * FROM COLLECTIONS WHERE NAME='{}'", name));
  return (col.size() > 0);
}

bool SongDatabase::collectionExists(int id)
{
  nlohmann::json col = getSqlJson(std::format("SELECT * FROM COLLECTIONS WHERE ID='{}'", id));
  return (col.size() > 0);
}

bool SongDatabase::songExists(int id)
{
  return (getSong(id).size() > 0);
}

int SongDatabase::removeSong(int id)
{

  std::string query  = "SELECT * FROM SONGS WHERE ID=" + std::to_string(id) + ";";
  nlohmann::json sql_json = getSqlJson(query);

  if (sql_json == nullptr)  {
    return 1;
  }

  if (sql_json != nullptr; sql_json.size() == 1)  {
      query  = "DELETE FROM SONGS WHERE ID=" + std::to_string(id) + ";";
      sqlite3_exec(DB, query.c_str(), nullptr, nullptr, nullptr);
      return 0;
  }
  else {
    // TODO fatal error! There cannot be two items with same ID!
    return 2;
  }
}

int SongDatabase::addCollection(std::string name)
{

    // obtain unique ID
    nlohmann::json sql_json = getSqlJson("SELECT * FROM COLLECTIONS ORDER BY ID DESC LIMIT 1;");
    
    std::string available_id = (sql_json.empty()) ? "0" : sql_json[0]["ID"].get<std::string>();

    try
    {
      available_id = std::to_string(std::stoi(available_id) + 1);
    }
    catch(const std::exception& e)
    {
      return 2;
    }

    std::ostringstream query;

    if (collectionExists(name))  {
      SongBookUtils::printError("Collection already exists...");
      return 3;
    }
    else {
      query << "INSERT INTO COLLECTIONS VALUES(" << available_id << ",\n";
      query << "'" << name << "'";
      query << ");";
    }


    char* messageError;
    int exit = sqlite3_exec(DB, query.str().c_str(), sql_cb, nullptr, &messageError);

    if (exit)  {
      SongBookUtils::printError(messageError);
      return 1;
    }

    return exit;
}
int SongDatabase::removeCollection(int id)
{
  std::string query  = "SELECT * FROM COLLECTIONS WHERE ID=" + std::to_string(id) + ";";
  nlohmann::json sql_json = getSqlJson(query);

  if (sql_json == nullptr)  {
    return 1;
  }

  if (sql_json != nullptr; sql_json.size() == 1)  {
      query  = "DELETE FROM COLLECTIONS WHERE ID=" + std::to_string(id) + ";";
      sqlite3_exec(DB, query.c_str(), nullptr, nullptr, nullptr);
      return 0;
  }
  else {
    // TODO fatal error! There cannot be two items with same ID!
    return 2;
  }
}

std::string SongDatabase::getCollection(int id)
{
  nlohmann::json collections = getCollections();
  std::string s_id;

  if (collections.count(s_id))  {
    return collections.at(s_id);
  } else {
    return "";
  }
}

int SongDatabase::addSongToCollection(int song_id, int collection_id)
{
  if (!songExists(song_id)) {
    return 1;
  }
  else if (!collectionExists(collection_id))  {
    return 2;
  }    

  std::ostringstream query;

  query << "INSERT INTO COLLECTION_MAP (SONG_ID, COLLECTION_ID) ";
  query << "VALUES (" << song_id << ", " << collection_id << ");";

  char* messageError;
  int exit = sqlite3_exec(DB, query.str().c_str(), sql_cb, nullptr, &messageError);
  
  if (exit)  {
    SongBookUtils::printError(messageError);
    return 3;
  }
  
  return 0;

}

int SongDatabase::removeSongFromCollection(int song_id, int collection_id)
{
  if (!songExists(song_id)) {
    return 1;
  }
  else if (!collectionExists(collection_id))  {
    return 2;
  }    

  std::ostringstream query;

  query << "DELETE FROM COLLECTION_MAP ";
  query << "WHERE SONG_ID=" << song_id << " AND COLLECTION_ID=" << collection_id << ");";

  char* messageError;
  int exit = sqlite3_exec(DB, query.str().c_str(), sql_cb, nullptr, &messageError);
  
  if (exit)  {
    SongBookUtils::printError(messageError);
    return 3;
  }
  
  return 0;
}

bool SongDatabase::isInCollection(int song_id, int collection_id)
{
  if (!songExists(song_id)) {
    return false;
  }
  else if (!collectionExists(collection_id))  {
    return false;
  }    

  std::ostringstream query;

  query << "SELECT * FROM COLLECTION_MAP ";
  query << "WHERE SONG_ID=" << song_id << " AND COLLECTION_ID=" << collection_id << ");";
  

  nlohmann::json j = getSqlJson(query.str());

  return (j.size()>1) ? true : false;
}

int SongDatabase::getCollectionCount(int collection_id)
{
  
  std::ostringstream query;

  query << "SELECT COUNT(*) AS song_count ";
  query << "FROM COLLECTION_MAP ";
  query << "WHERE COLLECTION_ID=" << collection_id << ";";

  char* messageError;
  int exit = sqlite3_exec(DB, query.str().c_str(), sql_cb, nullptr, &messageError);

  if (exit)  {
    SongBookUtils::printError(messageError);
    return -1;
  }


  int cnt = stoi(sql_contents[0]["song_count"]);

  sql_contents.clear();
  return cnt;

}