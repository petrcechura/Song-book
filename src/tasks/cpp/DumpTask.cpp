#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <format>
#include "DumpTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include <map>
#include <vector>
#include "json.hpp"

int DumpTask::executeCommand(int error_code)
{

  /*
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
    */
    return error_code;
}

int DumpTask::startInteractive()
{
  SongBookUtils::getInstance()->printInteractive("Choose a format in which you wish to dump contents. Available are:");
  for (auto const& [format, formatter] : this->formatters)  {
    SongBookUtils::getInstance()->printInteractive(format, 2);
  }

  std::string fmt = parent->getInput(1);
  if (!this->formatters.contains(fmt)) {
    return UNSUPPORTED_FORMAT;
  }
  else {
	  std::vector<std::string> v_fmt = {fmt};
	  arg_store_t arg_fmt = {true, v_fmt};
    updateArgument("-format", arg_fmt);
  }

  SongBookUtils::getInstance()->printInteractive("Do you wish to choose the collection of songs to dump? (y/n)");
  if (parent->getInput(1) == "y") {
    // TODO
  }

  return SUCCESS;
}

void DumpTask::endInteractive(int error_code)
{
  switch(error_code)
  {
    case SUCCESS:
      SongBookUtils::getInstance()->printInteractive(std::format("Successfuly dumped Songbook contents into \'{}\' directory", SongBookUtils::getInstance()->getConfigItem("paths/bard_dir")));
      break;
    case GENERATE_SONGBOOK_FAILED:
      SongBookUtils::getInstance()->printInteractive("Failed to generate a songbook..."); 
      break;
    case UNSUPPORTED_FORMAT:
      SongBookUtils::getInstance()->printInteractive("Unsupported format!");
      break;
  }
}
