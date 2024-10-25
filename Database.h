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
        int song_count;
        std::map<int, Song*> song_container;
		std::string fname;

    public:

        Database(std::string fname = "database.json");
        ~Database();

        int addSong(Song *song);
        int addSong(std::string json_string);
        int addSong(json_t json_string);
        int removeSong(std::string name);
        int removeSong(int id);
		int removeSong(Song* song);

        Song* getSong(int id);

        int sort(std::string criteria);

        int loadJsonFile(std::string fname);
		int saveJsonFile();

        json_t getJson();
        std::string getJsonString() {return this->getJson().dump(); };

};



#endif
