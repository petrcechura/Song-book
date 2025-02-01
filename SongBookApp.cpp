#include <string>
#include <iostream>
#include <iomanip>
#include "SongBookApp.h"
#include "tasks/tasks.h"
#include "lib/cmdapp/cmdapp.h"

SongBookApp::SongBookApp()
{
  database = new Database;

  AddTask* add = new AddTask("add", this);
  RemoveTask* remove = new RemoveTask("remove", this);
  ModifyTask* modify = new ModifyTask("modify", this);
  HelpTask* help = new HelpTask("help", this);
  SortTask* sort = new SortTask("sort", this);
  FindTask* find = new FindTask("find", this);
  ListTask* list = new ListTask("list", this);

  this->addTask(add);
  this->addTask(remove);
  this->addTask(modify);
  this->addTask(help);
  this->addTask(sort);
  this->addTask(list);
}









