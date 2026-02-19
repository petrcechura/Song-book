#include <string>
#include <iostream>
#include <sstream>
#include <format>
#include <cstdio>
#include <iomanip>
#include "ModifyTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "SongEditorServer.hpp"

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
				song["LYRICS"] = values2string(getArgument("-lyrics"));
				if (song["LYRICS"] == "")  {
					song.erase("LYRICS");
				}

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
	SongBookUtils::getInstance()->printInteractive("Type an ID of song you want to modify");
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
			SongBookUtils::getInstance()->printInteractive(std::format("'{}' does not contain valid ID to parse!", str_id), 1);
			return INVALID_ID;
		}
		std::vector<std::string> s = {str_id.data()};
		updateArgument("-id", {false, s});
		
	}
	
	if (parent->getDatabase()->songExists(id))  {

		nlohmann::json song = parent->getDatabase()->getSong(id);
		
		SongBookUtils::getInstance()->printSongListHeader();
		SongBookUtils::getInstance()->printSong(song["NO"], song["ID"], song["TITLE"], song["ARTIST"], song["LYRICS"] != "NULL");
		
		SongBookUtils::getInstance()->printInteractive("Type new song name (leave blank for no modification)", 2);
		std::string name = parent->getInput(1);
		SongBookUtils::getInstance()->printInteractive("Type new artist name (leave blank for no modification)", 2);
		std::string author = parent->getInput(1);
		SongBookUtils::getInstance()->printInteractive("Do you want to modify lyrics as well? (y/n)");
		std::string response = parent->getInput(1);
		std::string lyrics = "";

		if (response == "y")  {
			std::string editor = SongBookUtils::getInstance()->getConfigItem("commons/text_editor");
			if (editor == "")  {
				SongBookUtils::getInstance()->printInteractive("Unable to edit lyrics, text editor not found under commons/text_editor!", 1);
			}
			else if (!SongBookUtils::getInstance()->systemCommandExists(editor.c_str())){
				SongBookUtils::getInstance()->printInteractive("Unable to edit lyrics, command `" + editor + "` does not exist!", 1);
			}
			else {
				std::string tmpfile = std::tmpnam(nullptr);

				lyrics = SongBookUtils::getInstance()->sql2txt(song["LYRICS"].get<std::string>());
				
				std::ostringstream cmd_oss;
				if (lyrics != "NULL")  {
					cmd_oss << "echo \'" 
							<< lyrics
							<< "\' > "
							<< tmpfile
							<< " && ";
				}
				cmd_oss << editor
						<< " "
						<< tmpfile;
				
				std::string cmd = cmd_oss.str();

				system(cmd.c_str());
				cmd = std::format("cat {}", tmpfile);
				lyrics = SongBookUtils::getInstance()->execSystemCommand(cmd.c_str());
			}
		}

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
		v = {lyrics};
		updateArgument("-lyrics", {false, v});

		return SUCCESS;

	}
	else  {
		SongBookUtils::getInstance()->printInteractive(std::format("Song with ID {} does not exist!", id), 1);
		return SONG_NOT_FOUND;
	}
}

void ModifyTask::endInteractive(int error_code)
{
	switch(error_code)
	{
	  	case SUCCESS:
			SongBookUtils::getInstance()->printInteractive("Song has been modified succesfully...", 1); break;
      	case OK_EXIT_CHAR: break;
      	case INVALID_ID:
			SongBookUtils::getInstance()->printInteractive("Invalid ID passed as an argument", 1); break;
      	case SONG_NOT_FOUND:
			SongBookUtils::getInstance()->printInteractive("Could not find a song with this title and name in database!", 1); break;
      	case NO_ID:
			SongBookUtils::getInstance()->printInteractive("Missing -id as an argument...", 1); break;
      	case MISSING_ARGS:
			SongBookUtils::getInstance()->printInteractive("Missing arguments (required -id, -title, -artist)", 1); break;
      	case DB_ERROR:
			SongBookUtils::getInstance()->printInteractive("Could not modify a song due to internal error in database...", 1); break;
		default:
			SongBookUtils::getInstance()->printInteractive("Could not modify a song due to unknown error...", 1); break;
	}
}
