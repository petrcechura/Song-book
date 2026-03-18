#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "ListTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include <format>
#include "json.hpp"
#include <ncurses.h>

void ListTask::listTable()
{
	switch(parent->getState())  {
	  case SongBookApp::app_state_t::SONG_BROWSE:
		if (select_song > stoi(SongBookUtils::getConfigItem("workspace/song_count")))  {
			select_song = 0;
			upper_song_no = 0;
			lower_song_no = MAX_ITEMS_PER_PAGE;
		}
		listSongs();
	  break;
	  case SongBookApp::app_state_t::COLLECTION_BROWSE:
		if (select_collection > stoi(SongBookUtils::getConfigItem("workspace/collection_count")))  {
			select_collection = 0;
			upper_collection_no = 0;
			lower_collection_no = MAX_ITEMS_PER_PAGE;
		}
		listCollections();
	  break;
	}
}

void ListTask::listSongs()
{ 

	nlohmann::json data = SongBookUtils::getInstance()->getConfigJson("workspace/songs");

	windows["Main Screen"]->Clear();

	std::string s = printSongListHeader();
	windows["Main Screen"]->Print(s, 21, -1, true);
	
	int i = 0;
	for(const auto& [key, item] : data.items())  {
		if (i > upper_song_no && i < lower_song_no)  {

			if (i == select_song)  {
				windows["Main Screen"]->Print(printSong(item), 1, 1, true);
				SongBookUtils::getInstance()->setConfigItem("workspace/current_song_id", item["ID"]);
			}
			else {
				windows["Main Screen"]->Print(printSong(item));
			}
		}
		i++;
	}

	windows["Main Screen"]->Refresh();
}

void ListTask::listCollections()
{ 

	nlohmann::json data = SongBookUtils::getInstance()->getConfigJson("workspace/collections");

	windows["Main Screen"]->Clear();

	std::string s = printCollectionListHeader();
	windows["Main Screen"]->Print(s, 21, -1, true);
	
	int i = 0;
	for(const auto& [key, item] : data.items())  {
		if (i > upper_collection_no && i < lower_collection_no)  {

			if (i == select_collection)  {
				windows["Main Screen"]->Print(printCollection(item), 1, 1, true);
				SongBookUtils::getInstance()->setConfigItem("workspace/current_collection_id", item["ID"]);
			}
			else {
				windows["Main Screen"]->Print(printCollection(item));
			}
		}
		i++;
	}

	windows["Main Screen"]->Refresh();
}

int ListTask::Execute(char command)
{	
    switch(command)  {
      case 'w': moveUp();
                break;
      case 's': moveDown();
			    break;
	  case 'v': tableLeft();
	  			break;
	  case 'b': tableRight();
	  			break;
	  case '-': break;
      default: return 1;
    }

	listTable();

    return 0;
}

void ListTask::moveDown()
{	

	int max_cnt;
	switch(parent->getState())
	{
		case SongBookApp::app_state_t::SONG_BROWSE:
			SongBookUtils::printError("before stoi");
			max_cnt = stoi(SongBookUtils::getConfigItem("workspace/song_count"));
			SongBookUtils::printError("after stoi");
			if (select_song < max_cnt-1) 
				select_song++;
			if (select_song == lower_song_no)  {
				lower_song_no++;
				upper_song_no++;
			}
			break;
		case SongBookApp::app_state_t::COLLECTION_BROWSE:
			max_cnt = stoi(SongBookUtils::getConfigItem("workspace/collection_count"));
			if (select_collection < max_cnt-1) 
				select_collection++;
			if (select_collection == lower_collection_no)  {
				lower_collection_no++;
				upper_collection_no++;
			}
			break;
	}
}

void ListTask::moveUp()
{
	switch(parent->getState())
	{
		case SongBookApp::app_state_t::SONG_BROWSE:
			if (select_song > 0) 
				select_song--;
			if (select_song == upper_song_no)  {
				lower_song_no--;
				upper_song_no--;
			}
			break;
		case SongBookApp::app_state_t::COLLECTION_BROWSE:
			if (select_collection > 0) 
				select_collection--;
			if (select_collection == upper_collection_no)  {
				lower_collection_no--;
				upper_collection_no--;
			}
			break;
	}
}

void ListTask::tableLeft()
{
	if (parent->getState() == SongBookApp::app_state_t::SONG_BROWSE)  {
		parent->setState(SongBookApp::app_state_t::COLLECTION_BROWSE);
	}
	else if (parent->getState() == SongBookApp::app_state_t::COLLECTION_BROWSE)  {
		parent->setState(SongBookApp::app_state_t::SONG_BROWSE);
	}
}

void ListTask::tableRight()
{
	if (parent->getState() == SongBookApp::app_state_t::SONG_BROWSE)  {
		parent->setState(SongBookApp::app_state_t::COLLECTION_BROWSE);
	}
	else if (parent->getState() == SongBookApp::app_state_t::COLLECTION_BROWSE)  {
		parent->setState(SongBookApp::app_state_t::SONG_BROWSE);
	}
}

std::string ListTask::printSong(const nlohmann::json& song)
{
  
  std::string title =   song.count("TITLE")   ? song.at("TITLE") : "NULL";
  std::string artist =  song.count("ARTIST")  ? song.at("ARTIST") : "NULL";
  std::string id =      song.count("ID")      ? song.at("ID") : "NULL";
  std::string no =      song.count("NO")      ? song.at("NO") : "NULL";
  bool has_lyrics =     song.count("LYRICS") ? !(song["LYRICS"] == "NULL") : false;

  std::ostringstream string_cont;
	string_cont << std::setw(4) << std::left << no
				<< std::left << SongBookUtils::alignString(title, 	' ', TITLE_WIDTH)
				<< std::left << SongBookUtils::alignString(artist,  ' ', ARTIST_WIDTH)
                << "    " << (has_lyrics ? "X" : " ");

  return string_cont.str().c_str();
}

std::string ListTask::printCollection(const nlohmann::json& collection)
{
  std::string id    =    collection.count("ID")      ? collection.at("ID") : "NULL";
  std::string name  =    collection.count("NAME")    ? collection.at("NAME") : "NULL";
  std::string count =    "UNKNOWN";

  std::ostringstream string_cont;
  string_cont << std::setw(4) << std::left << id;
  string_cont << std::left << SongBookUtils::alignString(name, ' ', TITLE_WIDTH);
  string_cont << std::left << SongBookUtils::alignString(count, ' ', ARTIST_WIDTH);

  return string_cont.str().c_str();
}

std::string ListTask::printCollectionListHeader()
{
  std::ostringstream string_cont;

  string_cont << std::setw(4) << std::left << "ID"
		        << std::setw(TITLE_WIDTH) <<  "Collection"
		        << std::setw(ARTIST_WIDTH) << "Song Count";

  return string_cont.str().c_str();
}

std::string ListTask::printSongListHeader()
{
  std::ostringstream string_cont;

	string_cont << std::setw(4) << std::left << "NO"
			    << std::setw(TITLE_WIDTH) << "Title "
			    << std::setw(ARTIST_WIDTH) << "Artist "
              	<< "Has lyrics?";

  return string_cont.str().c_str();
}

std::string ListTask::printSongListBottom()
{
  std::ostringstream string_cont;
  string_cont << std::string(4 + 4 + TITLE_WIDTH + ARTIST_WIDTH+ 11, '-') << std::endl;

  return string_cont.str().c_str();
}