#ifndef SONGDATABASE_H
#define SONGDATABASE_H

#include <vector>
#include <map>
#include "json.hpp"
#include <string>
#include <sqlite3.h>

class SongDatabase
{

    private:
        sqlite3 *DB;
        /** config holding paths and other settings for database */
        nlohmann::json config;
        
        /** Own private implementation of `compare` function, supporting czech characters
         *
         *  Returns 1 when order is right (first -> second), -1 otherwise, or 0 when strings match */
        int compare(std::string firstString, std::string secondString);
        nlohmann::json getSqlJson(std::string query = "");

        std::string properties[3] = {
                    "TITLE",
                    "ARTIST",
                    "LYRICS"
        };

        std::string order = "ID";

    public:

        SongDatabase(nlohmann::json _config);
        ~SongDatabase();

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

        std::string convert_to_ascii(std::string str);

        nlohmann::json getSong(int id);
        nlohmann::json getSong(std::string title, std::string artist="");

        bool songExists(std::string title, std::string artist="");
        bool songExists(int id);

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
