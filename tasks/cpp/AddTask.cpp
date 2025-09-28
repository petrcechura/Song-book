#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "AddTask.h"
#include "SongBookApp.h"

int AddTask::executeCommand(int error_code)
{	
	if (error_code)  {
		return error_code;
	}

	if (argumentExists("-title", true) && argumentExists("-artist", true))  {
		
		nlohmann::json song = nlohmann::json();
		
		arg_store_t arg = getArgument("-title");
		song["TITLE"] = values2string(arg);
		arg = getArgument("-artist");
		song["ARTIST"] = values2string(arg);
		arg = getArgument("-force");
		song["LYRICS"] = "";

		int err = parent->getDatabase()->addSong(song.dump(), arg.isTrue);

		if (err == 0)  {
			return SUCCESS;
		}
		else  {
			return ERR_ADD_SONG_FAILED;
		}
	}
	else {
		return ERR_MISSING_ARGS;
	}
}

int AddTask::startInteractive()
{
	std::string name;
	std::string author;

	std::string cmd_str;
	
	parent->printInteractive("Type a song name");
	name = parent->getInput(1);

	if (name.size() > TITLE_WIDTH)  {
		parent->printInteractive(std::format("Name {} is too long! Max width is {} chars, your's {}", name, TITLE_WIDTH, name.size()), 1);
		return ERR_TITLE_TOO_LONG;
	}
	else if (name == "")  {
		parent->printInteractive("Name field shall not be empty string!", 1);
		return ERR_TITLE_FIELD_EMPTY;
	}
	else if (name == std::string(1,parent->getExitChar()) )  {
		return OK_EXIT_CHAR;
	}
  	else if (parent->getDatabase()->songExists(name))  {
		nlohmann::json song = parent->getDatabase()->getSong(name);
		parent->printInteractive("Song with this name already exists!", 1);
		parent->printSongListHeader();
		parent->printSong(song["ID"], song["TITLE"], song["ARTIST"]);
		parent->printSongListBottom();

		parent->printInteractive("... Do you really with to proceed? (y/n)", 1);
    	std::string response = parent->getInput(1);

    	if (response != "y")  {
        	return ERR_SONG_EXISTS;
    	}
		else {
			this->updateArgument("-force", {true, {}});
		}
  	}

	parent->printInteractive("Type an artist's name");
	author = parent->getInput(1);

	if (author.size() > ARTIST_WIDTH)  {
		parent->printInteractive(
			std::format("Artist's name '{}' is too long! Max width is {} characters, your's {}...", author, ARTIST_WIDTH, author.size()), 1);
		return ERR_ARTIST_TOO_LONG;
	}
	else if (author == std::string(1,parent->getExitChar()) )  {
		return OK_EXIT_CHAR;
	}

	std::vector<std::string> t = {name.data()};
	arg_store_t arg_title = {true, t};
	this->updateArgument("-title", arg_title);
	t = {author.data()};
	arg_store_t arg_author = {true, t};
	this->updateArgument("-artist", arg_author);

	return SUCCESS;
}

void AddTask::endInteractive(int error_code)  {

	switch(error_code)
	{
		case SUCCESS:
			parent->printInteractive("Song added sucessfully...", 1); break;
		case ERR_TITLE_TOO_LONG:
			parent->printInteractive("Title field too long! Can't add this song...", 1); break;
		case ERR_TITLE_FIELD_EMPTY:
			parent->printInteractive("Title field cannot be empty...", 1); break;
		case OK_EXIT_CHAR:
			break;
		case ERR_SONG_EXISTS:
			parent->printInteractive("Song with these properties already exists...", 1); break;
		case ERR_ARTIST_TOO_LONG:
			parent->printInteractive("Artist field too long! Can't add this song...", 1); break;
		case ERR_ADD_SONG_FAILED:
			parent->printInteractive("Failed to add a song due to internal error...", 1); break;
		case ERR_MISSING_ARGS:
			parent->printInteractive("Arguments -title, -artist are missing, can't add this song...", 1); break;
		default:
			parent->printInteractive("Failed to add a song due to unknown error code...", 1); break;
	}
}


