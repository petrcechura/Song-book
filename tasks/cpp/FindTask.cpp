#include <string>
#include <iostream>
#include <iomanip>
#include "../h/FindTask.h"
#include "../../SongBookApp.h"
#include "../../lib/json/json.hpp"

int FindTask::Start()
{
  using json_t = nlohmann::json;
  
  std::string regex;
  json_t data;

  std::cout << "Type a pattern to find\n\t>>";
  std::getline(std::cin, regex);
  
  data = parent->getDatabase()->findSong(regex);

  std::cout << "Matched songs:" << std::endl;
	for(const auto& [key, item] : data.items())  {
    std::cout << key << " - " << item << std::endl;
	}

  return 0;
}
