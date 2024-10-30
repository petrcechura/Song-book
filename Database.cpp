#include "json/json.hpp"
#include <vector>
#include <map>
#include "Song.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include "Database.h"

Database::Database(std::string fname)  {
    this->song_container = std::map<int, Song*>();
    this->song_count = 0;
	this->fname = fname;
}

Database::~Database()  {
    if (!this)
        delete this;
}


int Database::addSong(Song* song)  {

    if (song->getName() == "")  {
        // TODO
    }
    if (song->getAuthor() == "")  {
        // TODO
    }

    song->setId(song_count);
    this->song_container[song_count] = song;
    song_count++;

    return 0;
}

int Database::addSong(std::string json_string)  {
    return 1;
}

Database::json_t Database::getJson()  {

    Database::json_t j = json_t();

    for (auto song : song_container)  {
        Database::json_t _j  = song.second->getJson();
            // TODO throw an error, if smth went wrong
        j[std::to_string(song.first)] = _j;
    }

    return j;
}

Song* Database::getSong(int id)
{
    if (auto search = song_container.find(id); search != song_container.end())  {
        return search->second;
    }
    else  {
		return nullptr;
    }
}

int Database::sort(std::string criteria)  
{
    return 0;
}

int Database::saveJsonFile()
{
	std::ofstream file(this->fname);
	
	file << getJsonString();

	file.close();

	return 0;

}

int Database::loadJsonFile(std::string fname)
{	
	std::ifstream file(fname);

	if (file.fail())  {
		return 1;
	}
	
	json_t data = json_t::parse(file);

	// TODO check proper format

	if (data.is_discarded())  {
		return 2;
	}

	for (auto &el : data.items())  {
		Song* song = new Song();
		song->loadJson(el.value());
		addSong(song);
	}
	
	return 0;

}


// TODO
int Database::removeSong(Song* song)
{
	//TODO
	return 1;
}


// TODO
int Database::removeSong(int id)
{
	int removed = song_container.erase(id);

	if (removed == 1)  {
		// remove was successfull
		return 0;
	}
	else if (removed == 0)  {
		// element with this ID does not exist!
		return 1;
	}
	else  {
		// more than one element removed! FATAL ERROR
		return 2;
	}

}
