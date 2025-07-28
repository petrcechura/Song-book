#include "../lib/json/json.hpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sqlite3.h>
#include <filesystem>
#include "Database.h"

static std::vector<std::map<std::string, std::string>> sql_contents;

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

int Database::SendQuery(std::string query)  {
    char* messageError;
    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, 0, &messageError);

    std::cout << "exit code: " << exit << std::endl;
    return 0;
}

int Database::changeOrder(std::string order)  {
    // Convert to uppercase
    for (int i = 0; i < order.size(); i++)  {
      order[i] = std::toupper(order[i]);
    }

    // Check if property is present...
    bool isProp = false;
    for (const auto& prop : properties)  {
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

Database::Database(std::string fname, std::string backupDir)  {
    this->backupDir = backupDir;

    int exit = sqlite3_open(DB_file, &DB);

    if (exit) {
        std::cout << "Couldn't open SQL database `" << DB_file << "`!" << std::endl;
    }
    else {

      std::string sql = "CREATE TABLE SONGS(ID INT PRIMARY KEY NOT NULL";

      for (const auto& c : properties)  {
          sql += ",\n" + c + "TEXT NOT NULL";
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

Database::~Database()  {
    if (!this) {
      delete this;
    }
}

inline std::string convert_to_ascii(std::string str)  {
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


int Database::compare(std::string firstString, std::string secondString)  {

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


int Database::addSong(std::string json_string, bool override)  {

    nlohmann::json sql_json = getSqlJson("SELECT * FROM SONGS ORDER BY ID DESC LIMIT 1;");

    nlohmann::json j = nlohmann::json::parse(json_string);

    std::string query;

    // if input json contains ID that already exists inside database and override is on, don't add, just modify
    if (j.count("ID") && (getSqlJson("SELECT * FROM SONGS WHERE ID = " + j.at("ID").dump()) != "null") && override)  {
        query = "UPDATE SONGS SET ";
        int i = 0;
        for (const auto& p : properties) {
            if (j.count(p))  {
                query += (i++ ? ",\n" : "\n") + p + " = " + j[p].dump();
            }
        }
        query += "\n WHERE ID=" + j["ID"].dump() + ";";
      
    }
    // else generate incremental ID and assign new song to database
    else {
        int id;
        if (sql_json.dump() == "null")  {
            id = 0;
        }
        else {
            id = std::stoi(sql_json[0]["ID"].get<std::string>()) + 1;
        }
        
        query = "INSERT INTO SONGS VALUES(" + std::to_string(id);
        for (const auto& p : properties) {
            query += ", " + j[p].dump() + "";
        }
        query += ");";
    }

    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, nullptr, nullptr);

    return exit;
}

/** Get json representation of sql contents */
nlohmann::json Database::getSqlJson(std::string query)  {

    if (query == "") {
        query = "SELECT * FROM SONGS ORDER BY " + order;
    }

    int exit = sqlite3_exec(DB, query.c_str(), sql_cb, nullptr, nullptr);

    nlohmann::json data = nlohmann::json();

    if (exit)  {
        std::cout << "Error getting sql json!" << std::endl;
        std::cout << data.dump() << std::endl;
        return data;
    }

    int i = 0;
    for (const auto& content : sql_contents)  {
        nlohmann::json item  = nlohmann::json();

        item["ID"] = content.at("ID");

        for (const auto& p : properties)  {
          if (content.count(p))  {
            item[p] = content.at(p);
          }
        }

        data[i++] = item;
    }

    sql_contents.clear();

    return data;
}

nlohmann::json Database::getJson()
{
    return getSqlJson();
}

nlohmann::json Database::getSong(int id)
{   
    std::string query = "SELECT * FROM SONGS WHERE ID=" + std::to_string(id) + ";";
    nlohmann::json sql_json = getSqlJson(query);

    if (sql_json.size() != 1)  {
      // TODO fatal error! There cannot be two items with same ID!
    }

    return sql_json[0];
}

nlohmann::json Database::getSong(std::string name)
{   
    std::string query = "SELECT * FROM SONGS WHERE TITLE = " + name + ";";
    std::cout << query << std::endl;
    nlohmann::json sql_json = getSqlJson(query);

    return sql_json[0];
}

nlohmann::json Database::findSong(std::string pattern)
{
  std::string query = "SELECT * FROM SONGS WHERE TITLE LIKE '%" + pattern + "%';";

  return getSqlJson(query);
}

int Database::removeSong(int id)
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


int Database::makeBackup()
{
  time_t timestamp;
  time(&timestamp);
  
  // TODO make it universal for OS
  std::string backup_name = this->backupDir + "/" + "backup-" + ctime(&timestamp);
  
  // if the `database file` does not exist, return 1
  if (!std::filesystem::exists(DB_file))  {
    return 1;
  }
  
  // if the backup dir does not exist, create it
  if (!std::filesystem::exists(backupDir))  {
    std::cout << "creating backup dir " << backupDir << " ...\n";
    std::filesystem::create_directories(backupDir);
  }

  std::filesystem::copy_file(DB_file, backup_name);

  return 0;

}
