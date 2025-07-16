#include <string>
#include <iostream>
#include <iomanip>
#include "FindTask.h"
#include "SongBookApp.h"
#include "json.hpp"

int FindTask::Start()
{
  using json_t = nlohmann::json;
  
  std::string regex;
  json_t data;

  std::cout << "Type a pattern to find\n\t>>";
  std::getline(std::cin, regex);
  
  data = parent->getDatabase()->findSong(regex);

  std::cout << "Matched songs:" << std::endl;
  std::cout << data.dump() << std::endl;

  std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
			  << std::endl;	
	std::cout << std::setw(4) << std::left << "ID"
			  << std::setw(TITLE_WIDTH) << "Title "
			  << std::setw(ARTIST_WIDTH) << "Artist "
			  << std::endl;
	std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
			  << std::endl;

	for(const auto& [key, item] : data.items())  {
    parent->printSong(item.at("ID"), item.at("TITLE"), item.at("ARTIST"));
	}

  std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
	  	  << std::endl;

  return 0;
}
