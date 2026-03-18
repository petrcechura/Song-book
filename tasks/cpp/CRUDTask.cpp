#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "CRUDTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "SongEditorServer.hpp"

int CRUDTask::Execute(char command)
{	
	switch(parent->getState())  {
		case SongBookApp::app_state_t::SONG_BROWSE:
      		switch(command)  {
        		case 'a': addSong();
						  break;
				case 'm': modifySong();
						  break;
				case 'd': deleteSong();
						  break;
        		default: return 1;
      		}
		break;
	}

    return 0;
}

void CRUDTask::modifySong()
{

	windows["Log Screen"]->Clear();
	int id = stoi(SongBookUtils::getInstance()->getConfigItem("workspace/current_song_id", "-1"));

	if (parent->getDatabase()->songExists(id))  {
		nlohmann::json song = parent->getDatabase()->getSong(id);

		windows["Log Screen"]->Print(std::format("Modifying a song: ({}, {})", song["TITLE"].get<std::string>(), song["ARTIST"].get<std::string>()));
		windows["Log Screen"]->Print("Type a new song title (leave blank for no change)");
		std::string title = windows["Log Screen"]->GetString();

		if (!title.empty())  {
			song["TITLE"] = title;
		}

		windows["Log Screen"]->Print("Type a new song artist (leave blank for no change)");
		std::string artist = windows["Log Screen"]->GetString();

		if (!artist.empty())  {
			song["ARTIST"] = artist;
		}

		windows["Log Screen"]->Print("Do you want to modify lyrics as well? (y/n)");
		std::string choice = windows["Log Screen"]->GetString();

		if (choice == "y")  {
			windows["Main Screen"]->Clear();
			song["LYRICS"] = parent->SongEditor(song["LYRICS"]);
		}

		int exit = parent->getDatabase()->addSong(song, true);
		if (!exit) {
			windows["Log Screen"]->Print("Song successfully modified...");
		}
		else {
			windows["Log Screen"]->Print("Error occured when modifying a song...");
		}
	}
	else {
		windows["Log Screen"]->Print(std::format("Unexpected error! Song with ID '{}' does not exist...", id));
	}

	// update the song count in config json
  	std::string song_cnt = std::to_string(parent->getDatabase()->getJson().size());
  	SongBookUtils::setConfigItem("workspace/song_count", song_cnt);
  	nlohmann::json songs = parent->getDatabase()->getJson();
  	SongBookUtils::setConfigJson("workspace/songs", songs);

}

void CRUDTask::deleteSong()
{

	windows["Log Screen"]->Clear();

	int id = stoi(SongBookUtils::getInstance()->getConfigItem("workspace/current_song_id", "-1"));

	if (parent->getDatabase()->songExists(id))  {
		windows["Log Screen"]->Print("Are you sure you want to delete this song? (y/n)");

		std::string choice = windows["Log Screen"]->GetString();

		if (choice == "y")  {
			int error = parent->getDatabase()->removeSong(id);
			if (!error)  {
				windows["Log Screen"]->Print("Song successfully deleted...");
				
				// update the song count in config json
  				std::string song_cnt = std::to_string(parent->getDatabase()->getJson().size());
  				SongBookUtils::setConfigItem("workspace/song_count", song_cnt);
			}
			else {
				windows["Log Screen"]->Print("Error occured when deleting a song...");
			}
		}
	}
	else {
		windows["Log Screen"]->Print(std::format("Song with id '{}' does not exist!", id));
	}

	// update the song count in config json
  	std::string song_cnt = std::to_string(parent->getDatabase()->getJson().size());
  	SongBookUtils::setConfigItem("workspace/song_count", song_cnt);
  	nlohmann::json songs = parent->getDatabase()->getJson();
  	SongBookUtils::setConfigJson("workspace/songs", songs);

}


void CRUDTask::addSong()
{	
	windows["Log Screen"]->Clear();

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
  	nlohmann::json songs = parent->getDatabase()->getJson();
  	SongBookUtils::setConfigJson("workspace/songs", songs);
}