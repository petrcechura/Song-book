#include <string>
#include <iostream>
#include <iomanip>
#include "FindTask.h"
#include "SongBookApp.h"
#include "json.hpp"

int FindTask::startInteractive()
{
  using json_t = nlohmann::json;
  
  std::string regex;
  json_t data;

  parent->printInteractive("Type a pattern to find");
  std::getline(std::cin, regex);

  std::vector<std::string> v = {regex};
  this->updateArgument("-pattern", {false, v});

  return 0;
}

int FindTask::executeCommand()
{
  if (this->argumentExists("-pattern", true))  {
    arg_store_t a = this->getArgument("-pattern");
    nlohmann::json data = parent->getDatabase()->findSong(a.values[0]);

    parent->printInteractive("Matched songs");
    parent->printSongListHeader();
    for(const auto& [key, item] : data.items())  {
      parent->printSong(item.at("ID"), item.at("TITLE"), item.at("ARTIST"));
	  }
    parent->printSongListBottom();
    return 0;
  }
  else {
    return 1;
  }

  return 0;
}