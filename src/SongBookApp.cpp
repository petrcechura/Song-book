#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "json.hpp"
#include "PrintTask.h"
#include <vector>
#include "WindowServer.h"


SongBookApp::SongBookApp(nlohmann::json _config)
{
  this->config = _config.size()>0 ? _config : nlohmann::json();
  database = new SongDatabase(this->config);

  std::string exit_char = SongBookUtils::getInstance()->getConfigItem("commons/exit_char");
  if (exit_char.empty())  {
    this->EXIT_CHAR = '-';
  } 
  else {
    this->EXIT_CHAR = exit_char[0];
  }

  // ==================================
  // ====== INIT WINDOWS ==============
  // ==================================
  Window* main_window = new Window("Main Screen", 100, 20, 5, 2);
  Window* log_window = new Window("Log Screen", 100, 10, 5, 23);

  this->AddWindow(main_window);
  this->AddWindow(log_window);

  PrintTask* print_task = new PrintTask("Print", this, "descr");
  print_task->AddWindow(main_window);
  print_task->AddWindow(log_window);




  this->AddTask(print_task);

  //AddTask* add = new AddTask("add", this);
  //add->setDescription("Adds a new song to database");
  //add->updateArgument("-title");
  //add->updateArgument("-artist");
  //add->updateArgument("-collection");
  //add->updateArgument("-force");
//
  //RemoveTask* remove = new RemoveTask("remove", this);
  //remove->setDescription("Removes a song from database");
  //remove->updateArgument("-id");
//
  //ModifyTask* modify = new ModifyTask("modify", this);
  //modify->setDescription("Modifies song in database");
  //modify->updateArgument("-id");
//
  //
  //GatherTask* gather = new GatherTask("gather", this);
  //gather->setDescription("Tries to gather lyrics for a song");
  //gather->updateArgument("-id");
  //
//
  //HelpTask* help = new HelpTask("help", this);
  //help->setDescription("Shows this message");
//
  //SortTask* sort = new SortTask("sort", this);
  //sort->setDescription("Sorts database by chosen criteria");
  //sort->updateArgument("-criteria");
//
  //FindTask* find = new FindTask("find", this);
  //find->setDescription("Tries to find a song matching given regex");
  //find->updateArgument("-pattern");
//
  //ListTask* list = new ListTask("list", this);
  //list->setDescription("Shows all songs in a database");
  //
  //LatexTask* latex = new LatexTask("latex", this);
  //latex->setDescription("Exports a database into a LaTeX file");
//
  //ExitTask* exit = new ExitTask("exit", this);
  //exit->setDescription("Exits a program");
//
  //DumpTask* dump = new DumpTask("dump", this);
  //dump->setDescription("Dumps the database in selected format into directory to be processed by pdf generation app");
  //dump->updateArgument("-format");
  //dump->updateArgument("-selection");
  //dump->updateArgument("-out_dir");
//
  //TestTask* test = new TestTask("query", this);
  //test->setDescription("TODO");
//
  //this->addTask(add);
  //this->addTask(remove);
  //this->addTask(modify);
  //this->addTask(help);
  //this->addTask(sort);
  //this->addTask(list);
  //this->addTask(find);
  //this->addTask(latex);
  //this->addTask(exit);
  //this->addTask(test);
  //this->addTask(dump);
//
  //if (gather->checkSanity()) {
  //  this->addTask(gather);
  //}
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

