#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "json.hpp"
#include "PrintTask.h"
#include "ListTask.h"
#include "CRUDTask.h"
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
  Window* main_window = new Window("Main Screen", 110, 20, 5, 2);
  Window* log_window = new Window("Log Screen", 110, 10, 5, 23);
  Window* commands_window = new Window("Commands Screen", 10, 20, 117, 2);

  this->AddWindow(main_window);
  this->AddWindow(log_window);
  this->AddWindow(commands_window);

  PrintTask* print_task = new PrintTask("Print", this, "descr");
  print_task->AddWindow(main_window);
  print_task->AddWindow(log_window);

  ListTask* list_task = new ListTask("List", this, "descr");
  list_task->AddWindow(main_window);
  list_task->AddWindow(log_window);

  CRUDTask* crud_task = new CRUDTask("Crud", this, "descr");
  crud_task->AddWindow(main_window);
  crud_task->AddWindow(log_window);


  GatherTask* gather_task = new GatherTask("gather", this, "descr");
  gather_task->AddWindow(main_window);
  gather_task->AddWindow(log_window);

  this->AddTask(print_task);
  this->AddTask(list_task);
  this->AddTask(crud_task);
  this->AddTask(gather_task);

  // set up config
  std::string song_cnt = std::to_string(this->database->getJson().size());
  SongBookUtils::setConfigItem("workspace/song_count", song_cnt);

}
void SongBookApp::StartHook()
{

}

void SongBookApp::StopHook()
{
	std::cout << "exiting program..." << std::endl;
	exit(0);
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

