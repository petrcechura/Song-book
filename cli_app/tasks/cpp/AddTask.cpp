#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "AddTask.h"
#include "SongBookApp.h"

int AddTask::executeCommand()
{	
	if (argumentExists("-title", true) && argumentExists("-artist", true))  {
		arg_store_t t = getArgument("-title");
		arg_store_t a = getArgument("-artist");
		
		nlohmann::json song = nlohmann::json();
		song["TITLE"] = t.values[0];
		song["ARTIST"] = a.values[0];
		return parent->getDatabase()->addSong(song.dump());
	}
	else {
		return 1;
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
		return 1;
	}
	else if (name == "")  {
		parent->printInteractive("Name field shall not be empty string!", 1);
		return 2;
	}
	else if (name == std::string(1,parent->getExitChar()) )  {
		return 0;
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
        	return 2;
    	}
  	}

	parent->printInteractive("Type an artist's name");
	author = parent->getInput(1);

	if (author.size() > ARTIST_WIDTH)  {
		parent->printInteractive(
			std::format("Artist's name '{}' is too long! Max width is {} characters, your's {}...", author, ARTIST_WIDTH, author.size()), 1);
		return 1;
	}
	else if (author == std::string(1,parent->getExitChar()) )  {
		return 0;
	}

	std::vector<std::string> t = {name.data()};
	arg_store_t arg_title = {true, t};
	this->updateArgument("-title", arg_title);
	t = {author.data()};
	arg_store_t arg_author = {true, t};
	this->updateArgument("-artist", arg_author);

	return 0;
}

void AddTask::endInteractive(int error_code)  {
	if (!error_code)  {
		parent->printInteractive("Song added succesfully...", 1);
	}
	else  {
		parent->printInteractive("Could not save song to database!", 1);
	}
}


