#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "CRUDTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"

int CRUDTask::Execute(char command)
{
      switch(command)  {
        case 'a': addSong();
				  break;
		case 'm': modifySong();
				  break;
		case 'd': deleteSong();
				  break;
        default: return 1;
      }

      return 0;
}

void CRUDTask::modifySong()
{
	windows["Log Screen"]->Print("`modifySong` feature not yet implemented!");
	return;
}

void CRUDTask::deleteSong()
{
	windows["Log Screen"]->Print("`deleteSong` feature not yet implemented!");
	return;
}


void CRUDTask::addSong()
{
	std::string name;
	std::string author;

	windows["Log Screen"]->Print("Type a song name:");
	name = windows["Log Screen"]->GetString(30);

	if (name.empty())  {
		windows["Log Screen"]->Print("Invalid Name inserted, terminating...");
		return;
	}
	else if (parent->getDatabase()->songExists(name))  {
		windows["Log Screen"]->Print("Song with this name already exists!");
		nlohmann::json song = parent->getDatabase()->getSong(name);
		windows["Log Screen"]->Print(std::format("Song: ({}, {}, {})", 
													song["ID"].get<std::string>(), 
													song["TITLE"].get<std::string>(),
													song["ARTIST"].get<std::string>()));
		windows["Log Screen"]->Print("Do you really wish to proceed? (y/n)");
		char c = getch();

		if (c != 'y')  {
			return;
		}
	}

	windows["Log Screen"]->Print("Type an artist's name:");
	author = windows["Log Screen"]->GetString(30);
	
	if (author.empty())  {
		windows["Log Screen"]->Print("Invalid author's name inserted, terminating...");
		return;
	}

	nlohmann::json song = nlohmann::json();
	
	song["TITLE"] = name;
	song["ARTIST"] = author;
	int err = parent->getDatabase()->addSong(song, true);
	windows["Log Screen"]->Print(std::format("INPUT: {} {} (error code = {})", name, author, err));

	if (!err)  {
		windows["Log Screen"]->Print(std::format("Song with name {} successfuly added to database", name));
	}
	else  {
		windows["Log Screen"]->Print("Unexpected error occured when adding song to database...");
	}

	// update the song count in config json
  	std::string song_cnt = std::to_string(parent->getDatabase()->getJson().size());
  	SongBookUtils::setConfigItem("workspace/song_count", song_cnt);

	return;
}