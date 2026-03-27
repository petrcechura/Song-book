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

  nlohmann::json data = SongBookUtils::getConfigJson("workspace/songs");

  windows["Log Screen"]->Clear();
  windows["Log Screen"]->Print("Select a directory name for your songbook");
  std::string songbook = windows["Log Screen"]->GetString(20);

  if (songbook.empty())  {
    windows["Log Screen"]->Print("Cannot have songbook directory name!");
    return;
  }

  std::string work_dir = SongBookUtils::getConfigItem("paths/work_dir", "work");
  if (!std::filesystem::exists(work_dir))  {
    windows["Log Screen"]->Print(std::format("Directory `{}` does not exist! Creating...", work_dir));
    std::filesystem::create_directory(work_dir);
  }

  songbook = std::format("{}/{}", work_dir, songbook);

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
  
  int e = formatter->exportSongs(songbook.c_str());
  formatter->clearPages();

  std::string bard_tool = SongBookUtils::getConfigItem("paths/bard_command", "");

  if (bard_tool.empty())  {
    windows["Log Screen"]->Print("Cannot export songs to pdf file, `bard` tool not found!");
    return;
  }

  windows["Log Screen"]->Print("Creating PDF via formatter...");
  e = formatter->generateSongBook(songbook.c_str());
  switch(e)  {
    case 1: windows["Log Screen"]->Print("Bard tool path not found...");
      break;
    case 2: windows["Log Screen"]->Print("Output directory does not exist!");
      break;
    case 0: windows["Log Screen"]->Print("Done.");
      break;
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
