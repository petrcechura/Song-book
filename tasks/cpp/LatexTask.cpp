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

void LatexTask::exportToPdf()
{

  
  
  nlohmann::json data = parent->getDatabase()->getJson();

  int size = data.size();
  windows["Log Screen"]->Print(std::format("Exporting {} songs to pdf using external Bard tool", size));

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
    windows["Log Screen"]->Print("Error occured!");

  } else {
    windows["Log Screen"]->Print("Done.");
  }
}

int LatexTask::Execute(char command)
{
      switch(command)  {
        case 'l': exportToPdf();
				  break;
        default: return 1;
      }

      return 0;
}
