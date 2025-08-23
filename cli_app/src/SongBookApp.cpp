#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "SongBookApp.h"
#include "tasks.h"
#include <vector>
#include "cmdapputils.h"

SongBookApp::SongBookApp()
{
  database = new SongDatabase;

  AddTask* add = new AddTask("add", this);
  add->setDescription("Adds a new song to database");
  add->updateArgument("-title");
  add->updateArgument("-artist");
  add->updateArgument("-force");

  RemoveTask* remove = new RemoveTask("remove", this);
  remove->setDescription("Removes a song from database");

  ModifyTask* modify = new ModifyTask("modify", this);
  modify->setDescription("Modifies song in database");

  HelpTask* help = new HelpTask("help", this);
  help->setDescription("Shows this message");

  SortTask* sort = new SortTask("sort", this);
  sort->setDescription("Sorts database by chosen criteria");

  FindTask* find = new FindTask("find", this);
  find->setDescription("Tries to find a song matching given regex");
  find->updateArgument("-pattern");

  ListTask* list = new ListTask("list", this);
  list->setDescription("Shows all songs in a database");

  LatexTask* latex = new LatexTask("latex", this);
  latex->setDescription("Exports a database into a LaTeX file");

  ExitTask* exit = new ExitTask("exit", this);
  exit->setDescription("Exits a program");

  BackupTask* backup = new BackupTask("backup", this);
  backup->setDescription("TODO");

  FetchTask* fetch = new FetchTask("fetch", this);
  fetch->setDescription("TODO");

  PushTask* push = new PushTask("push", this);
  push->setDescription("TODO");

  TestTask* test = new TestTask("query", this);
  push->setDescription("TODO");

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
  this->addTask(test);
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
    str = _str + std::string(TITLE_WIDTH - SongBookApp::countStringChars(_str), fill);
  }

  return str;
}

void SongBookApp::startHook()
{

}

void SongBookApp::stopHook()
{
	std::cout << "exiting program..." << std::endl;
	exit(0);
}

void SongBookApp::executeCommands(std::string cmd_line, bool exitWhenDone)
{
  // TODO this funtion shall parse cmd into chain of tasks with their arguments and then run them.
    std::vector<std::string> cmd_chain;
    int pos = 0;
    while(pos < cmd_line.size()){
      pos = cmd_line.find(";");
      cmd_chain.push_back(cmd_line.substr(0,pos));
      cmd_line.erase(0,pos+1);
    }
    
    for (const auto& c : cmd_chain)  {
      std::string cmd = c.substr(0, c.find(" "));
      if (this->tasks.count(cmd))  {
        tasks.at(cmd)->parseCommand(c);
        tasks.at(cmd)->executeCommand();
      }
      else {
        std::cout << "Unknown command '" << cmd << "'!" << std::endl;
      }
    }

  if (exitWhenDone)  {
  }
}


void SongBookApp::printSong(const std::string& id, const std::string& name, const std::string& author)
{
  
	std::cout << std::setw(4) << std::left << id
				    << std::left << SongBookApp::alignString(name, 	' ', TITLE_WIDTH)
				    << std::left << SongBookApp::alignString(author, ' ', ARTIST_WIDTH)
				    << std::endl;
}

void SongBookApp::printSongListHeader()
{
  std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
			  << std::endl;	
	std::cout << std::setw(4) << std::left << "ID"
			  << std::setw(TITLE_WIDTH) << "Title "
			  << std::setw(ARTIST_WIDTH) << "Artist "
			  << std::endl;
	std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+')
			  << std::endl;
}

void SongBookApp::printSongListBottom()
{
  std::cout << std::string(4 + TITLE_WIDTH + ARTIST_WIDTH, '+') << std::endl;
}

void SongBookApp::printInteractive(const std::string& text, unsigned int indentation, bool newline)
{
  std::string t = "#";
  t += std::string(indentation, '#');
  
  std::cout << t << " " <<  text;
  if (newline) 
    std::cout << std::endl;
}