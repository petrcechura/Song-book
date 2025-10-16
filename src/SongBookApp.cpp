#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "json.hpp"
#include "tasks.h"
#include <vector>
#include "cmdapputils.h"

SongBookApp::SongBookApp(nlohmann::json _config)
{
  this->config = _config.size()>0 ? _config : nlohmann::json();
  database = new SongDatabase(this->config);

  std::string exit_char = SongBookUtils::getInstance()->getConfigItem("commons/exit_char");
  if (exit_char.empty())  {
    this->EXIT_CHAR = '-';
  } 
  else {
    this->EXIT_CHAR = exit_char[0];
  }

  AddTask* add = new AddTask("add", this);
  add->setDescription("Adds a new song to database");
  add->updateArgument("-title");
  add->updateArgument("-artist");
  add->updateArgument("-force");

  RemoveTask* remove = new RemoveTask("remove", this);
  remove->setDescription("Removes a song from database");
  remove->updateArgument("-id");

  ModifyTask* modify = new ModifyTask("modify", this);
  modify->setDescription("Modifies song in database");
  modify->updateArgument("-id");

  
  GatherTask* gather = new GatherTask("gather", this);
  gather->setDescription("Tries to gather lyrics for a song");
  gather->updateArgument("-id");
  

  HelpTask* help = new HelpTask("help", this);
  help->setDescription("Shows this message");

  SortTask* sort = new SortTask("sort", this);
  sort->setDescription("Sorts database by chosen criteria");
  sort->updateArgument("-criteria");

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

  TestTask* test = new TestTask("query", this);
  test->setDescription("TODO");

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
  this->addTask(test);

  if (gather->checkSanity()) {
    this->addTask(gather);
  }
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

void SongBookApp::executeCommands(std::string commands_string, bool exitWhenDone)
{
    std::vector<std::string> commands_vec;
    std::string command;

    // gather all commands into vector
    for (int i = 0; i < commands_string.size(); i++)  {
      if (commands_string[i] == ';')  {
        if (!command.empty())  {
          commands_vec.push_back(command);
          command.clear();
        }
      }
      else {
        command += commands_string[i];
      }
    }
    if (!command.empty())  {
      commands_vec.push_back(command);
    }

    for (const auto& c : commands_vec)  {
      if (executeTaskString(c))  {
        std::cout << "Could not execute command '" << c << "'" << std::endl;
      }
    }
}


void SongBookApp::printSong(const std::string& no,
                            const std::string& id, 
                            const std::string& name, 
                            const std::string& author, 
                            bool has_lyrics)
{
  
	std::cout << std::setw(4) << std::left << no
            << std::setw(4) << std::left << id
				    << std::left << SongBookApp::alignString(name, 	' ', TITLE_WIDTH)
				    << std::left << SongBookApp::alignString(author, ' ', ARTIST_WIDTH)
            << "    " << (has_lyrics ? "X" : " ")
				    << std::endl;
}

void SongBookApp::printSongListHeader()
{
  std::cout << std::string(4 + 4 + TITLE_WIDTH + ARTIST_WIDTH + 11, '+')
			  << std::endl;	
	std::cout << std::setw(4) << std::left << "NO"
            << std::setw(4) << std::left << "ID"
			      << std::setw(TITLE_WIDTH) << "Title "
			      << std::setw(ARTIST_WIDTH) << "Artist "
            << "Has lyrics?"
			      << std::endl;
	std::cout << std::string(4 + 4 + TITLE_WIDTH + ARTIST_WIDTH + 11, '+')
			      << std::endl;
}

void SongBookApp::printSongListBottom()
{
  std::cout << std::string(4 + 4 + TITLE_WIDTH + ARTIST_WIDTH+ 11, '+') << std::endl;
}

void SongBookApp::printInteractive(const std::string& text, unsigned int indentation, bool newline)
{
  std::string t = "#";
  t += std::string(indentation, '#');
  
  std::cout << t << " " <<  text;
  if (newline) 
    std::cout << std::endl;
}