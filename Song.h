#ifndef SONG_H
#define SONG_H

#include <string>
#include "lib/json/json.hpp"


class Song
{   

    using json_t = nlohmann::json;
    
    public:
        Song(std::string name, std::string author, json_t flags = "");
		// for some reason, the std::map requires constructor with 0 args
		Song() {};

        ~Song();

        int getId() const { return id; };
        std::string getName() const { return name; };
        std::string getAuthor() const { return author; };

        json_t getJson() const;
		std::string getJsonString() const { return this->getJson().dump(); };

        void setId(int id);
        void setName(std::string name);
        void setAuthor(std::string author);
        void setFlags(json_t flags);

		void loadJson(json_t j);

    private:
        
        int id;
        std::string name;
        std::string author;
        json_t flags;
};

#endif
