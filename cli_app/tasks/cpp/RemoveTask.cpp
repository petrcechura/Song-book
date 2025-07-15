#include <string>
#include <iostream>
#include <iomanip>
#include "RemoveTask.h"
#include "json.hpp"
#include "SongBookApp.h"


int RemoveTask::Start()
{
	std::string str_id;
	std::cout << "Type an ID of song you want to modify\n\t>>";
	std::getline(std::cin, str_id);

	int id;

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return 0;
  	}

	try {
		id = std::stoi(str_id);
	}
	catch (const std::invalid_argument& e)  {
		std::cout << "'" << str_id << "' does not contain valid ID to parse!" << std::endl;
		return 1;
	}

	nlohmann::json song = parent->getDatabase()->getSong(id);
	int exit = parent->getDatabase()->removeSong(id);
	
	if (exit == 0)  { 	
		std::cout << "Song " << (song.count("TITLE") ? song.at("TITLE") : "NULL") << " removed..." << std::endl;
	}
	else if (exit == 1)  {
		std::cout << "Song with ID " << id << " does not exist!" << std::endl;
	}
	else if (exit == 2)  {
		std::cout << "FATAL ERROR: There are two songs with same ID!" << std::endl;
	}

	return exit;
}
