#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "ListTask.h"
#include "SongBookApp.h"
#include "json.hpp"

int ListTask::Start(bool interactive)
{ 
	nlohmann::json data = parent->getDatabase()->getJson();

	std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
			  << std::endl;	
	std::cout << std::setw(4) << std::left << "ID"
			  << std::setw(TITLE_WIDTH) << "Title "
			  << std::setw(ARTIST_WIDTH) << "Artist "
			  << std::endl;
	std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
			  << std::endl;
	
	for(const auto& [key, item] : data.items())  {
		std::string title = item.count("TITLE") ? item.at("TITLE") : "NULL";
		std::string artist = item.count("ARTIST") ? item.at("ARTIST") : "NULL";
		std::string id = item.count("ID") ? item.at("ID") : "NULL";
    	SongBookApp::printSong(id, title, artist);
	}

	std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
			  << std::endl;

	return 0;
}
