#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "ListTask.h"
#include "SongBookApp.h"
#include "json.hpp"

int ListTask::executeCommand(int error_code)
{ 
	nlohmann::json data = parent->getDatabase()->getJson();

	parent->printSongListHeader();
	
	for(const auto& [key, item] : data.items())  {
		std::string title = item.count("TITLE") ? item.at("TITLE") : "NULL";
		std::string artist = item.count("ARTIST") ? item.at("ARTIST") : "NULL";
		std::string id = item.count("ID") ? item.at("ID") : "NULL";
		std::string no = item.count("NO") ? item.at("NO") : "NULL";
		bool has_lyrics = item.count("LYRICS") ? !(item["LYRICS"] == "NULL") : false;
    	SongBookApp::printSong(no, id, title, artist, has_lyrics);
	}

	parent->printSongListBottom();

	return 0;
}
