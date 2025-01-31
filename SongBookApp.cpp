#include <string>
#include <iostream>
#include <iomanip>
#include "SongBookApp.h"

SongBookApp::SongBookApp()
{
  AddTask* add = new AddTask("add");
  RemoveTask* remove = new RemoveTask("remove");
  ModifyTask* modify = new ModifyTask("modify");

  this->addTask(add);
  this->addTask(remove);
  this->addTask(modify);
}


int AddTask::Start()
{
  std::cout << "TO BE IMPLEMENTED" << std::endl;
  return 1;
}


int ModifyTask::Start()
{
  std::cout << "TO BE IMPLEMENTED" << std::endl;
  return 1;
}


int RemoveTask::Start()
{
  std::cout << "TO BE IMPLEMENTED" << std::endl;
  return 1;
}
