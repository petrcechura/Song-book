#include <string>
#include <iostream>
#include <iomanip>
#include "FindTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "json.hpp"

int FindTask::startInteractive()
{
  using json_t = nlohmann::json;
  
  std::string regex;
  json_t data;

  SongBookUtils::getInstance()->printInteractive("Type a pattern to find");
  std::getline(std::cin, regex);

  std::vector<std::string> v = {regex};
  this->updateArgument("-pattern", {false, v});

  return SUCCESS;
}

int FindTask::executeCommand(int error_code)
{
  if (this->argumentExists("-pattern", true))  {
    arg_store_t a = this->getArgument("-pattern");
    nlohmann::json data = parent->getDatabase()->findSong(a.values[0]);

    SongBookUtils::getInstance()->printInteractive("Matched songs");
    SongBookUtils::getInstance()->printSongListHeader();
    for(const auto& [key, item] : data.items())  {
      SongBookUtils::getInstance()->printSong(item.at("NO"), item.at("ID"), item.at("TITLE"), item.at("ARTIST"), item.count("LYRICS"));
	  }
    SongBookUtils::getInstance()->printSongListBottom();
    return SUCCESS;
  }
  else {
    return SONG_NOT_FOUND;
  }
}

void FindTask::endInteractive(int error_code)
{
  switch(error_code)
  {
    case SUCCESS:
      break;
    case SONG_NOT_FOUND:
      SongBookUtils::getInstance()->printInteractive("Could not find a song under this pattern..."); break;
  }
}