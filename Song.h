
#ifndef SONG_H
#define SONG_H

#include <string>
#include "json/json.hpp"


class Song
{   

    using json_t = nlohmann::json;
    
    public:
        Song(std::string name, std::string author, json_t flags = "");
		// for some reason, the std::map requires constructor with 0 args
		Song() {};

        ~Song();

        int getId(bool latexFormat = false) const;
        std::string getName(bool latexFormat = false) const;
        std::string getAuthor(bool latexFormat = false) const;

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

        inline void replace(std::string& str, const std::string& subj, const std::string& replace_by) const;
};

#endif
