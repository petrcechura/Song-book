#ifndef DATABASE_H
#define DATABASE_H

#include "Song.h"
#include <vector>
#include <map>
#include "json/json.hpp"
#include <string>

class Database
{
    using json_t = nlohmann::json;

    private:
        std::vector<Song*> song_container;
		    /** File name for a `database` json */
        std::string fname;
        /** Folder name for backup json files */
        std::string backupDir;
        
        /** Own private implementation of `compare` function, supporting czech characters
         *
         *  Returns 1 when order is right (first -> second), -1 otherwise, or 0 when strings match */
        int compare(std::string firstString, std::string secondString);

    public:

        Database(std::string fname = "database.json", std::string backupDir = "backups");
        ~Database();

        int addSong(Song *song);
        int addSong(std::string json_string);
        int addSong(json_t json_string);
        int removeSong(std::string name);
        int removeSong(int id);
        /** Creates a backup file of a json `database` into `backupDir`
         *
         *  Backup file has the following name format: <backup-(current time&date)>
         */
        int makeBackup();
        /** Takes a `regex` as a argument, 
         * returns json representation of songs that match this regex */
        json_t findSong(std::string pattern);
		    int removeSong(Song* song);
        

        Song* getSong(int id);
        Song* getSong(std::string name);

        int getSongCount() { return song_container.size(); };

        int sort(std::string criteria);

        int loadJsonFile(std::string fname);
		    int saveJsonFile();

        json_t getJson();
        std::string getJsonString() {return this->getJson().dump(); };

};



#endif
