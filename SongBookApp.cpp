#include <string>
#include <iostream>
#include <iomanip>
#include "SongBookApp.h"

SongBookApp::SongBookApp()
{
  Add add = Add();
  Remove remove = Remove();
  Modify modify = Modify();

  this->addTask(add);
  this->addTask(remove);
  this->addTask(modify);
}


int Add::Start()
{
  std::cout << "TO BE IMPLEMENTED" << std::endl;
  return 1;
}


int Modify::Start()
{
  std::cout << "TO BE IMPLEMENTED" << std::endl;
  return 1;
}


int Remove::Start()
{
  std::cout << "TO BE IMPLEMENTED" << std::endl;
  return 1;
}
