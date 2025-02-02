#include <string>
#include <iostream>
#include <iomanip>
#include "../h/HelpTask.h"
#include "../../SongBookApp.h"

int HelpTask::Start()
{

  for( const auto& c : parent->tasks)  {
    std::cout << "> " << c.second->getCmd() << ": " << c.second->getDescr() << std::endl;
  }

  return 1;
}



