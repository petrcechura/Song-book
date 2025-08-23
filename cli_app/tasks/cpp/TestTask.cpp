#include <string>
#include <iostream>
#include <iomanip>
#include "TestTask.h"
#include "SongBookApp.h"

int TestTask::startInteractive()
{
  std::string query;

  std::cout << "Query:\n\t>>";
	std::getline(std::cin, query);

  parent->getDatabase()->SendQuery(query);
  return 0;
}
