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
    	SongBookApp::printSong(id, title, artist);
	}

	parent->printSongListBottom();

	return 0;
}
