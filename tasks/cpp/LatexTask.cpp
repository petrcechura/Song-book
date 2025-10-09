#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <format>
#include "LatexTask.h"
#include "SongBookApp.h"
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

    if (song.count("LYRICS")) {
      formatter->addSongPage(title, artist, song.at("LYRICS"));
    }
  }
  formatter->generateSongBook();
  formatter->clearPages();

	return 0;
}
