#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <map>
#include "../lib/json/json.hpp"
#include <string>
#include <sqlite3.h>

class Database
{

    private:
        sqlite3 *DB;
        const char* DB_file = "db.sql";
        /** Folder name for backup json files */
        std::string backupDir;
        
        /** Own private implementation of `compare` function, supporting czech characters
         *
         *  Returns 1 when order is right (first -> second), -1 otherwise, or 0 when strings match */
        int compare(std::string firstString, std::string secondString);
        nlohmann::json getSqlJson(std::string query = "");

        std::string properties[2] = {
                    "TITLE",
                    "ARTIST"
        };

        std::string order = "ID";

        enum ERROR_CODE 
        {
            SUCCESS,
            SQL_INSERT_ERROR,
            SQL_SELECT_ERROR
        };

    public:

        Database(std::string fname = "database.json", std::string backupDir = "backups");
        ~Database();

        int addSong(std::string json_string, bool override=false);
        int removeSong(int id);
        /** Creates a backup file of a json `database` into `backupDir`
         *
         *  Backup file has the following name format: <backup-(current time&date)>
         */
        int makeBackup();
        /** Takes a `regex` as a argument, 
         * returns json representation of songs that match this regex */
        nlohmann::json findSong(std::string pattern);

        nlohmann::json getSong(int id);
        nlohmann::json getSong(std::string name);

        int SendQuery(std::string query);
        
        // TODO
        int getSongCount() { return getJson().size(); };

        int loadJsonFile(std::string fname);
		int saveJsonFile();

        nlohmann::json getJson();
        std::string getJsonString() {return this->getJson().dump(); };

        int changeOrder(std::string order);
        

};



#endif
