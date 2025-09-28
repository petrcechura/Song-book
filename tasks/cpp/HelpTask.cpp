#include <string>
#include <iostream>
#include <iomanip>
#include "HelpTask.h"
#include "SongBookApp.h"

int HelpTask::executeCommand(int error_code)
{
  std::cout << "This is a simple Command-line application SongBookApp that allows you"
               "to communicate with a database of a songs (add new, delete old...).\n"
               "Application is controlled via commands called `tasks`, and is easily"
               "extendable for your own implementations." << std::endl;
  std::cout << "\nList of currently available `tasks`:" << std::endl;
  for( const auto& c : parent->tasks)  {
    std::cout << " > " << c.second->getCommandString() << ": " << c.second->getDescription() << std::endl;
  }

  std::cout << "Simply type a `task` and follow its instructions :)\n" << std::endl;

  std::cout << "\nTo exit a program, type '" << parent->getExitChar() << "'.\n" << std::endl;

  return 1;
}

int HelpTask::startInteractive()
{
  return 1;
}

