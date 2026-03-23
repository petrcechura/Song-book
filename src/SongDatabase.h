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
        
        /** Own private implementation of `compare` function, supporting czech characters
         *
         *  Returns 1 when order is right (first -> second), -1 otherwise, or 0 when strings match */
        int compare(std::string firstString, std::string secondString);
        nlohmann::json getSqlJson(std::string query = "");

        std::string song_properties[3] = {"TITLE", "ARTIST", "LYRICS"};

        std::string order = "ID";

        static int czech_collation(void*,
                           int len1, const void* str1,
                           int len2, const void* str2);

    public:

        SongDatabase();
        ~SongDatabase();
        
        // ==========================
        // ******* SONGS ************
        // ==========================
        int addSong(nlohmann::json json_string, bool override=false);
        int removeSong(int id);
        /** Takes a `regex` as a argument, 
         * returns json representation of songs that match this regex */
        nlohmann::json findSong(std::string pattern);

        nlohmann::json getSong(int id);
        nlohmann::json getSong(std::string title, std::string artist="");

        bool songExists(std::string title, std::string artist="");
        bool songExists(int id);

        // ==========================
        // ******* COLLECTIONS ******
        // ==========================
        /** Returns all collections that are present in database currently */
        nlohmann::json getCollections();
        std::string getCollection(int id);

        bool collectionExists(std::string name);
        bool collectionExists(int id);

        int SendQuery(std::string query);
        int addCollection(std::string name);
        int removeCollection(int id);
        
        // TODO
        int addSongToCollection(int song_id, int collection_id);
        int removeSongFromCollection(int song_id, int collection_id);
        bool isInCollection(int song_id, int collection_id);
        int getCollectionCount(int collection_id);
        
        // TODO
        int getSongCount() { return getJson().size(); };

        int loadJsonFile(std::string fname);
		int saveJsonFile();

        nlohmann::json getJson();
        std::string getJsonString() {return this->getJson().dump(); };

        int changeOrder(std::string order);
};



#endif
