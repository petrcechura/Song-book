
#ifndef SONG_H
#define SONG_H

#include <string>
#include "json/json.hpp"


//TODO
struct t_flags {
    bool folk;
    bool metal;
    int difficulty; 
};


//TODO make it virtual class with main methods as pure virtual
class Song
{   

    using json_t = nlohmann::json;
    
    public:
        Song(std::string name, std::string author, json_t flags = "");
		// for some reason, the std::map requires constructor with 0 args
		Song() {};

        ~Song();

        int getId() { return id; };
        std::string getName() { return name; };
        std::string getAuthor() { return author; };

        json_t getJson();
		std::string getJsonString() { return this->getJson().dump(); };

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
