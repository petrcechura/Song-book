#include <string>
#include <iostream>
#include <format>
#include <iomanip>
#include "ModifyTask.h"
#include "SongBookApp.h"

int ModifyTask::executeCommand(int error_code)
{	
	int id;

	if (error_code == SUCCESS)  {
		if (argumentExists("-id", true) && argumentExists("-title", true) && argumentExists("-artist", true))  {
			try {
				id = std::stoi(getArgument("-id").values[0]);
			}
			catch (const std::invalid_argument& e)  {
				return NO_ID;
			}

			if (parent->getDatabase()->songExists(id))  {
				nlohmann::json song = parent->getDatabase()->getSong(id);
				song["TITLE"] = values2string(getArgument("-title"));
				song["ARTIST"] = values2string(getArgument("-artist"));

				int exit = parent->getDatabase()->addSong(song, true);
				if (!exit) {
					return SUCCESS;
				}
				else {
					return DB_ERROR;
				}
			}
			else {
				return SONG_NOT_FOUND;
			}
		}
		else {
			return MISSING_ARGS;
		}
	}
	else {
		return error_code;
	}
}

int ModifyTask::startInteractive()
{
	std::string str_id;
	parent->printInteractive("Type an ID of song you want to modify");
	str_id = parent->getInput(1);

	int id;

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return OK_EXIT_CHAR;
  	}
	else  {
		try {
			id = std::stoi(str_id);
		}
		catch (const std::invalid_argument& e)  {
			parent->printInteractive(std::format("'{}' does not contain valid ID to parse!", str_id), 1);
			return INVALID_ID;
		}
		std::vector<std::string> s = {str_id.data()};
		updateArgument("-id", {false, s});
		
	}
	
	if (parent->getDatabase()->songExists(id))  {

		nlohmann::json song = parent->getDatabase()->getSong(id);
		
		parent->printSongListHeader();
		parent->printSong(song["ID"], song["TITLE"], song["ARTIST"], song.count("LYRICS"));
		
		parent->printInteractive("Type new song name (leave blank for no modification)", 2);
		std::string name = parent->getInput(1);
		parent->printInteractive("Type new artist name (leave blank for no modification)", 2);
		std::string author = parent->getInput(1);

		if (name == "")  {
			name = song["TITLE"];
		}
		if (author == "")  {
			author = song["ARTIST"];
		}

		std::vector<std::string> v = {name};
		updateArgument("-title", {false, v});
		v = {author};
		updateArgument("-artist", {false, v});

		return SUCCESS;

	}
	else  {
		parent->printInteractive(std::format("Song with ID {} does not exist!", id), 1);
		return SONG_NOT_FOUND;
	}
}

void ModifyTask::endInteractive(int error_code)
{
	switch(error_code)
	{
	  	case SUCCESS:
			parent->printInteractive("Song has been modified succesfully...", 1); break;
      	case OK_EXIT_CHAR: break;
      	case INVALID_ID:
			parent->printInteractive("Invalid ID passed as an argument", 1); break;
      	case SONG_NOT_FOUND:
			parent->printInteractive("Could not find a song with this title and name in database!", 1); break;
      	case NO_ID:
			parent->printInteractive("Missing -id as an argument...", 1); break;
      	case MISSING_ARGS:
			parent->printInteractive("Missing arguments (required -id, -title, -artist)", 1); break;
      	case DB_ERROR:
			parent->printInteractive("Could not modify a song due to internal error in database...", 1); break;
		default:
			parent->printInteractive("Could not modify a song due to unknown error...", 1); break;
	}
}