#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "ListTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "json.hpp"
#include <ncurses.h>

void ListTask::listSongs()
{ 
	nlohmann::json data = parent->getDatabase()->getJson();

	windows["Main Screen"]->Clear();

	std::string s = SongBookUtils::getInstance()->printSongListHeader();
	windows["Main Screen"]->Print(s, 21, -1, true);
	
	int i = 0;
	for(const auto& [key, item] : data.items())  {
		if (i > up_no && i < down_no)  {
			std::string title = item.count("TITLE") ? item.at("TITLE") : "NULL";
			std::string artist = item.count("ARTIST") ? item.at("ARTIST") : "NULL";
			std::string id = item.count("ID") ? item.at("ID") : "NULL";
			std::string no = item.count("NO") ? item.at("NO") : "NULL";
			bool has_lyrics = item.count("LYRICS") ? !(item["LYRICS"] == "NULL") : false;

			if (i == select)  {
				windows["Main Screen"]->Print(SongBookUtils::getInstance()->printSong(no, title, artist, has_lyrics), 1, 1, true);
				this->current_id = stoi(id);
			}
			else {
				windows["Main Screen"]->Print(SongBookUtils::getInstance()->printSong(no, title, artist, has_lyrics));
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
				  listSongs();
                  break;
        case 's': moveDown();
				  listSongs();
				  break;
        default: return 1;
      }

      return 0;
}

void ListTask::moveDown()
{	
	static int SONGS_CNT = parent->getDatabase()->getJson().size();

	if (this->select < SONGS_CNT-1)  {
		this->select++;
	}

	if (this->select == down_no)  {
		down_no++;
		up_no++;
	}
}

void ListTask::moveUp()
{
	if (this->select > 0)  {
		this->select--;
	}
	
	if (this->select == up_no)  {
		up_no--;
		down_no--;
	}
}