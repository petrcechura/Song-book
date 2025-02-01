#include <string>
#include <iostream>
#include <iomanip>
#include "../h/HelpTask.h"
#include "../../SongBookApp.h"

int HelpTask::Start()
{
	std::cout << "Available commands:" << std::endl;
	std::cout << "> help: Shows this message" << std::endl;
	std::cout << "> add:  Adds a new song to database" << std::endl;
	std::cout << "> remove: Removes a song from database" << std::endl;
	std::cout << "> list: Shows all songs from database" << std::endl;
	std::cout << "> modify: Modifies songs in database" << std::endl;
	std::cout << "> latex: Exports database to a LaTeX file" << std::endl;
	std::cout << "> exit: Saves database into json file and exits program" << std::endl;
  std::cout << "> backup: Saves database into json file and makes a backup (saving current database into separate directory)" << std::endl;
  std::cout << "> sort: Sorts database by chosen criteria (available criteria: 'name', 'author')" << std::endl;
  std::cout << "> find: Tries to find a song matching given Regular Expression" << std::endl;

  return 1;
}



