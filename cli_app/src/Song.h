#ifndef SONG_H
#define SONG_H

#include <string>
#include "../lib/json/json.hpp"
#include <set>


class Song
{   

    using json_t = nlohmann::json;
    
    public:
        Song(std::string name, std::string author);
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
        void setFlag(std::string flag);
        void setFlags(json_t flags);
        int deleteFlag(std::string flag);

		void loadJson(json_t j);

    private:
        
        int id;
        std::string name;
        std::string author;
        std::set<std::string> flags;

        inline void replace(std::string& str, const std::string& subj, const std::string& replace_by) const;
};

#endif
