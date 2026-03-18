#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "json.hpp"
#include <map>
#include "ListTask.h"
#include "CRUDTask.h"
#include "LatexTask.h"
#include "FilterTask.h"
#include "CollectionTask.h"
#include <format>
#include "GatherTask.h"
#include <vector>
#include "WindowServer.h"


SongBookApp::SongBookApp()
{
  this->database = new SongDatabase();

  this->EXIT_CHAR = char(27);

  // ==================================
  // ====== INIT WINDOWS ==============
  // ==================================
                        //            name                         
  Window* main_window = new Window("Main Screen", 110,   20,   5,   2);
  Window* log_window = new Window("Log Screen", 110, 10, 5, 23);
  Window* commands_window = new Window("Commands Screen", 20, 20, 117, 2);
  Window* stats_window = new Window("Stats Screen", 20, 10, 117, 23);

  this->AddWindow(main_window);
  this->AddWindow(log_window);
  this->AddWindow(commands_window);
  this->AddWindow(stats_window);

  ListTask* list_task = new ListTask("List", this, "descr");
  list_task->AddWindow(main_window);
  list_task->AddWindow(log_window);
  task_legend["w"] = "move up";
  task_legend["s"] = "move down";

  CRUDTask* crud_task = new CRUDTask("Crud", this, "descr");
  crud_task->AddWindow(main_window);
  crud_task->AddWindow(log_window);
  task_legend["m"] = "modify song";
  task_legend["d"] = "delete song";
  task_legend["a"] = "add song";

  GatherTask* gather_task = new GatherTask("gather", this, "descr");
  gather_task->AddWindow(main_window);
  gather_task->AddWindow(log_window);
  task_legend["g"] = "gather song";

  LatexTask* latex_task = new LatexTask("latex", this, "descr");
  latex_task->AddWindow(main_window);
  latex_task->AddWindow(log_window);
  task_legend["l"] = "export (pdf) song";

  CollectionTask* collection_task = new CollectionTask("collection", this, "descr");
  collection_task->AddWindow(main_window);
  collection_task->AddWindow(log_window);

  FilterTask* filter_task = new FilterTask("filter", this, "descr");
  filter_task->AddWindow(main_window);
  filter_task->AddWindow(log_window);

  this->AddTask(list_task);
  this->AddTask(crud_task);
  this->AddTask(gather_task);
  this->AddTask(latex_task);
  this->AddTask(collection_task);
  this->AddTask(filter_task);

  nlohmann::json songs = getDatabase()->getJson();
  SongBookUtils::setConfigJson("workspace/songs", songs);
  SongBookUtils::setConfigItem("workspace/song_count", std::to_string(songs.size()));

  nlohmann::json collections = getDatabase()->getCollections();
  SongBookUtils::setConfigJson("workspace/collections", collections);
  SongBookUtils::setConfigItem("workspace/collection_count", std::to_string(collections.size()));
}

void SongBookApp::StartHook()
{
  SongBookUtils::getInstance()->setConfigItem("workspace/current_song_id", std::to_string(0));
  SongBookUtils::getInstance()->setConfigItem("workspace/current_collection_id", std::to_string(0));
  windows["Stats Screen"]->Print(std::format("Song Count: {}", SongBookUtils::getConfigItem("workspace/song_count")));
  windows["Stats Screen"]->Print(std::format("Col. Count: {}", SongBookUtils::getConfigItem("workspace/collection_count")));
  windows["Stats Screen"]->Print(std::format("Filter: '{}'", SongBookUtils::getConfigItem("workspace/filter_pattern", "")));
  windows["Stats Screen"]->Print(std::format("Collection: '{}'", SongBookUtils::getConfigItem("workspace/filter_collection", "")));
  windows["Stats Screen"]->Print(std::format("Sort: '{}'", SongBookUtils::getConfigItem("workspace/sort", "")));

  this->tasks["List"]->Execute('-');

  for (const auto& [k, v] : task_legend)  {
    windows["Commands Screen"]->Print(std::format("{} = {}", k, v));
  }
}

void SongBookApp::StopHook()
{

}

void SongBookApp::afterExecuteHook()
{
  this->tasks["List"]->Execute('-');

  windows["Commands Screen"]->Clear();
  for (const auto& [k, v] : task_legend)  {
    windows["Commands Screen"]->Print(std::format("{} = {}", k, v));
  }

  windows["Stats Screen"]->Clear();
  windows["Stats Screen"]->Print(std::format("Song Count: {}", SongBookUtils::getConfigItem("workspace/song_count")));
  windows["Stats Screen"]->Print(std::format("Col. Count: {}", SongBookUtils::getConfigItem("workspace/collection_count")));
  windows["Stats Screen"]->Print(std::format("Filter: '{}'", SongBookUtils::getConfigItem("workspace/filter_pattern", "")));
  windows["Stats Screen"]->Print(std::format("Collection: '{}'", SongBookUtils::getConfigItem("workspace/filter_collection", "")));
  windows["Stats Screen"]->Print(std::format("Sort: '{}'", SongBookUtils::getConfigItem("workspace/sort", "")));
}

void SongBookApp::executeCommands(std::string commands_string, bool exitWhenDone)
{ 
    std::vector<std::string> commands_vec;
    std::string command;

    // gather all commands into vector
    for (int i = 0; i < commands_string.size(); i++)  {
      if (commands_string[i] == ';')  {
        if (!command.empty())  {
          commands_vec.push_back(command);
          command.clear();
        }
      }
      else {
        command += commands_string[i];
      }
    }
    if (!command.empty())  {
      commands_vec.push_back(command);
    }

    for (const auto& c : commands_vec)  {
      if (executeTaskString(c))  {
        std::cout << "Could not execute command '" << c << "'" << std::endl;
      }
    }
}

std::string SongBookApp::SongEditor(std::string lyrics)
{
	std::string tmpfile = std::tmpnam(nullptr);
  std::ostringstream cmd_oss;

	std::string editor = SongBookUtils::getInstance()->getConfigItem("commons/text_editor");
	if (editor == "")  {
    windows["Log Screen"]->Print("Unable to edit lyrics, text editor not found under commons/text_editor!");
    return lyrics;
	}
	
	if (lyrics != "NULL")  {
		cmd_oss << "echo \'" 
				<< lyrics
				<< "\' > "
				<< tmpfile
				<< " && ";
	}
	cmd_oss << editor
			<< " "
			<< tmpfile;
	
	std::string cmd = cmd_oss.str();
	system(cmd.c_str());
	cmd = std::format("cat {}", tmpfile);
	lyrics = SongBookUtils::getInstance()->execSystemCommand(cmd.c_str());

	return lyrics;
}
