#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <format>
#include "LatexTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include <map>
#include "json.hpp"

int LatexTask::executeCommand(int error_code)
{

	nlohmann::json data = parent->getDatabase()->getJson();

  int i = 0;
  std::string title;
	std::string artist;
	std::string id;
  for (const auto& song : data)  {
    title = song.count("TITLE") ? song.at("TITLE") : "NULL";
    artist = song.count("ARTIST") ? song.at("ARTIST") : "NULL";

    if (song["LYRICS"] != "NULL") {
      formatter->addSongPage(song);
    }
  }
  
  int e = formatter->generateSongBook();
  formatter->clearPages();

  if (e)  {
    return GENERATE_SONGBOOK_FAILED;
  } else {
    return SUCCESS;
  }
}

void LatexTask::endInteractive(int error_code)
{
  switch(error_code)
  {
    case SUCCESS:
      SongBookUtils::getInstance()->printInteractive(std::format("Successfuly dumped Songbook contents into \'{}\' directory", SongBookUtils::getInstance()->getConfigItem("paths/bard_dir")));
      break;
    case GENERATE_SONGBOOK_FAILED:
      SongBookUtils::getInstance()->printInteractive("Failed to generate a songbook..."); 
      break;
  }
}
