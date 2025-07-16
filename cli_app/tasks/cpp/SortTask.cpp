#include <string>
#include <iostream>
#include <iomanip>
#include "SortTask.h"
#include "SongBookApp.h"


int SortTask::Start()
{
  std::string criteria;

  std::cout << "Type a sorting criteria\n\t>>";
  std::getline(std::cin, criteria);

  int i = parent->getDatabase()->changeOrder(criteria);

  if (!i)  {
    std::cout << "Sort made succesfully..." << std::endl;
  }  
  else  {
    std::cout << "Unavailable sort criteria..." << std::endl;
  }

  return 0;
}
