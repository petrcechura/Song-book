#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "SongBookApp.h"
#include "tasks.h"
#include "cmdapputils.h"

SongBookApp::SongBookApp()
{
  database = new Database;

  AddTask* add = new AddTask("add", this);
  add->setDescr("Adds a new song to database");

  RemoveTask* remove = new RemoveTask("remove", this);
  remove->setDescr("Removes a song from database");

  ModifyTask* modify = new ModifyTask("modify", this);
  modify->setDescr("Modifies song in database");

  HelpTask* help = new HelpTask("help", this);
  help->setDescr("Shows this message");

  SortTask* sort = new SortTask("sort", this);
  sort->setDescr("Sorts database by chosen criteria");

  FindTask* find = new FindTask("find", this);
  find->setDescr("Tries to find a song matching given regex");

  ListTask* list = new ListTask("list", this);
  list->setDescr("Shows all songs in a database");

  LatexTask* latex = new LatexTask("latex", this);
  latex->setDescr("Exports a database into a LaTeX file");

  ExitTask* exit = new ExitTask("exit", this);
  exit->setDescr("Exits a program");

  BackupTask* backup = new BackupTask("backup", this);
  backup->setDescr("TODO");

  FetchTask* fetch = new FetchTask("fetch", this);
  fetch->setDescr("TODO");

  PushTask* push = new PushTask("push", this);
  push->setDescr("TODO");

  this->addTask(add);
  this->addTask(remove);
  this->addTask(modify);
  this->addTask(help);
  this->addTask(sort);
  this->addTask(list);
  this->addTask(find);
  this->addTask(latex);
  this->addTask(exit);
  this->addTask(backup);
  this->addTask(fetch);
  this->addTask(push);
}


/** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
int SongBookApp::countStringChars(const std::string& _str)
{
  std::wstring str = std::wstring_convert<std::codecvt_utf8<wchar_t>>()
    .from_bytes(_str);
  return str.size();
}

/** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
std::string SongBookApp::alignString(const std::string& _str, char fill, int maxWidth)
{	
  std::string str = "CANT DISPLAY (too long)" + std::string(maxWidth-23, fill);

  if (_str.size() < maxWidth)  {
    str = _str + std::string(NAME_WIDTH - countStringChars(_str), fill);
  }

  return str;
}

void SongBookApp::startHook()
{
	if (database->loadJsonFile("database.json"))  {
			std::cout << "Error while trying to load 'database.json' file (It's either corrupted or does not exist)!" << std::endl;
	}
}

void SongBookApp::stopHook()
{
	database->saveJsonFile();
	std::cout << "exiting program..." << std::endl;
	exit(0);
}








