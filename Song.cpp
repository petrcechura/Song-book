#include "json/json.hpp"
#include <string>
#include <iostream>
#include "Song.h"


Song::Song(std::string name, std::string author, json_t flags)  {
    this->setName(name);
    this->setAuthor(author);

    this->setFlags(flags);
}


Song::~Song()  {
    if (!this)
        delete this;
}

Song::json_t Song::getJson()  {
    json_t j;

    j["name"] = getName();
    j["author"] = getAuthor();

    return j;
}


void Song::setId(int id)  {
    this->id = id;
}
void Song::setName(std::string name)  {
    this->name = name;
}
void Song::setAuthor(std::string author)  {
    this->author = author;
}

void Song::setFlags(json_t flags)  {
    // TODO maybe convert to an array and check supported and unsupported flags
    this->flags = flags;
}

void Song::loadJson(json_t j)
{	
	for (auto &el : j.items())  {
		if (el.key() == "name")  {
			this->setName(el.value());
		}
		else if (el.key() == "author")  {
			this->setAuthor(el.value());
		}
		else  {
			// TODO
		}
	}

}
