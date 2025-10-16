#include "json.hpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "SongBookUtils.h"
#include <sqlite3.h>
#include <filesystem>
#include "SongDatabase.h"

static std::vector<std::map<std::string, std::string>> sql_contents;

SongDatabase::SongDatabase(nlohmann::json _config)  {

    this->config = (_config.size()>0) ? _config : nlohmann::json();

    std::string db_file_path;
    if (config.contains("paths")) {
      if (config["paths"].contains("db_file_path"))  {
        db_file_path = config["paths"].at("db_file_path");
      }
    }
    else  {
      db_file_path = "db.sql";
    }

    int exit = sqlite3_open(db_file_path.c_str(), &DB);

    if (exit) {
        std::cout << "Couldn't open SQL database `" <<  db_file_path << "`!" << std::endl;
    }
    else {

      std::string sql = "CREATE TABLE SONGS(ID INT PRIMARY KEY NOT NULL";

      for (const auto& c : properties)  {
          sql += ",\n" + c.name + " " + c.type;
      }

      sql += " );";
      
      char* messaggeError;
      exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
      
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

int SongDatabase::SendQuery(std::string query)  {
    char* messageError;
    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, 0, &messageError);

    std::cout << "exit code: " << exit << std::endl;
    if (!exit)  {
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
    for (const auto& prop : properties)  {
      if (order == prop.name)  {
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


std::string SongDatabase::convert_to_ascii(std::string str)  {
std::map<wchar_t, char> cz_chars = {
    {L'Á', 'a'}, {L'á', 'a'},
    {L'Č', 'c'}, {L'č', 'c'},
    {L'Ď', 'd'}, {L'ď', 'd'},
    {L'É', 'e'}, {L'é', 'e'},
    {L'Ě', 'e'}, {L'ě', 'e'},
    {L'Í', 'i'}, {L'í', 'i'},
    {L'Ň', 'n'}, {L'ň', 'n'},
    {L'Ó', 'o'}, {L'ó', 'o'},
    {L'Ř', 'r'}, {L'ř', 'r'},
    {L'Š', 's'}, {L'š', 's'},
    {L'Ť', 't'}, {L'ť', 't'},
    {L'Ú', 'u'}, {L'ú', 'u'},
    {L'Ý', 'y'}, {L'ý', 'y'},
    {L'Ž', 'z'}, {L'ž', 'z'},
    {L'ů', 'u'}
};
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wstr = converter.from_bytes(str);

    for (int i = 0; i < wstr.size(); i++)  {
      // convert cz characters to ascii
      if (cz_chars.count(wstr[i]))  {
        wstr[i] = cz_chars[wstr[i]];
      }
      
      // convert capitals into low letters
      if (wstr[i] > 64 && wstr[i] < 91)  {
        wstr[i] += 32; 
      }
    }

  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter2;
  return converter2.to_bytes(wstr);
} 


int SongDatabase::compare(std::string firstString, std::string secondString)  {

    std::string str1 = convert_to_ascii(firstString);
    std::string str2 = convert_to_ascii(secondString);


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
        for (const auto& p : properties) {
          query += (i++ ? ",\n" : "\n") + (j.contains(p.name) ? (p.name + " = \'" + SongBookUtils::txt2sql(j[p.name].get<std::string>()) + "\'") : (p.name + " = NULL"));
        
        }
        query += "\n WHERE ID=" + song["ID"].get<std::string>() + ";";
        std::cout << query << std::endl;
      }
      else  {
      // song is already present and override is false, hence don't do anything...
      return 1;
      }
    }
    else {
      query = "INSERT INTO SONGS VALUES(" + available_id;
      for (const auto& p : properties) {
        query += (j.contains(p.name)) ? (", '" + SongBookUtils::txt2sql(j[p.name].get<std::string>()) + "'") : (", NULL");
      }
      query += ");";
    }
    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, nullptr, nullptr);

    return exit;
}

/** Get json representation of sql contents
 *  Returns json or null
 */
nlohmann::json SongDatabase::getSqlJson(std::string query)  {

    if (query == "") {
        query = "SELECT * FROM SONGS ORDER BY " + order;
    }

    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, nullptr, nullptr);

    nlohmann::json data = nlohmann::json();

    if (exit)  {
        return data;
    }

    int i = 0;
    for (const auto& content : sql_contents)  {
        nlohmann::json item  = nlohmann::json();

        item["ID"] = content.at("ID");
        item["NO"] = std::to_string(i);

        for (const auto& p : properties)  {
          if (content.count(p.name))  {
            item[p.name] = content.at(p.name);
          }
        }

        data[i++] = item;
    }

    sql_contents.clear();

    return data;
}

nlohmann::json SongDatabase::getJson()
{
    return getSqlJson();
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


int SongDatabase::makeBackup()
{
  time_t timestamp;
  time(&timestamp);

  std::string backupDir;
  
  
  if (config.contains("paths") && config["paths"].contains("backup_dir"))  {
      backupDir = config["paths"].at("backup_dir");
    }
    else  {
      backupDir = ".";
  }
  
  // TODO make it universal for OS
  std::string backup_name = backupDir + "/" + "backup-" + ctime(&timestamp);
  
  std::string db_file_path;
  if (config.contains("paths") && config["paths"].contains("db_file_path"))  {
    db_file_path = config["paths"].at("db_file_path");
  }
  else  {
    db_file_path = "db.sql";
  }
  // if the `database file` does not exist, return 1
  if (!std::filesystem::exists(db_file_path))  {
    return 1;
  }
  
  // if the backup dir does not exist, create it
  if (!std::filesystem::exists(backupDir))  {
    std::cout << "creating backup dir " << backupDir << " ...\n";
    std::filesystem::create_directories(backupDir);
  }

  std::filesystem::copy_file(db_file_path, backup_name);

  return 0;

}