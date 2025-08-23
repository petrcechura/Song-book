#include <string>
#include <iostream>
#include <format>
#include <iomanip>
#include "ModifyTask.h"
#include "SongBookApp.h"

int ModifyTask::executeCommand()
{	
	int id;
	if (argumentExists("-id", true) && argumentExists("-title", true) && argumentExists("-artist", true))  {
		try {
			id = std::stoi(getArgument("-id").values[0]);
		}
		catch (const std::invalid_argument& e)  {
			return 2;
		}
		
		if (parent->getDatabase()->songExists(id))  {
			nlohmann::json song = parent->getDatabase()->getSong(id);
			song["TITLE"] = values2string(getArgument("-title"));
			song["ARTIST"] = values2string(getArgument("-artist"));
			
			int exit = parent->getDatabase()->addSong(song.dump(),true);
			return exit;
		}
	}
	else {
		return 1;
	}

	return 1;
}

int ModifyTask::startInteractive()
{
	std::string str_id;
	parent->printInteractive("Type an ID of song you want to modify");
	str_id = parent->getInput(1);

	int id;

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return 0;
  	}
	else  {
		try {
			id = std::stoi(str_id);
		}
		catch (const std::invalid_argument& e)  {
			parent->printInteractive(std::format("'{}' does not contain valid ID to parse!", str_id), 1);
			return 1;
		}
		std::vector<std::string> s = {str_id.data()};
		updateArgument("-id", {false, s});
		
	}
	
	if (parent->getDatabase()->songExists(id))  {

		nlohmann::json song = parent->getDatabase()->getSong(id);
		
		parent->printSongListHeader();
		parent->printSong(song["ID"], song["TITLE"], song["ARTIST"]);
		
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

		return 0;

	}
	else  {
		parent->printInteractive(std::format("Song with ID {} does not exist!", id), 1);
		return 1;
	}
}

void ModifyTask::endInteractive(int error_code)
{
	if (!error_code)  {
		parent->printInteractive("Song has been modified succesfully...", 1);
	}
	else {
		parent->printInteractive("Failed to modify  ...", 1);
	}
}