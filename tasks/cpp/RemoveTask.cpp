#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "RemoveTask.h"
#include "json.hpp"
#include "SongBookApp.h"

int RemoveTask::executeCommand(int error_code)
{
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	else {
		if (argumentExists("-id", true))  {
			std::string str_id = getArgument("-id").values[0];
			int id;

			try {
				id = std::stoi(str_id);
			}
			catch (const std::invalid_argument& e)  {
				return INVALID_ID;
			}
			if (parent->getDatabase()->songExists(id))  {
				int error = parent->getDatabase()->removeSong(id);
				if (!error)  {
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
			return NO_ID;
		}
	}
}

int RemoveTask::startInteractive()
{
	std::string str_id;
	int id;
	parent->printInteractive("Type an ID of song you wish to remove");
	str_id = parent->getInput(1);

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return OK_EXIT_CHAR;
  	}
	else {
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

	return SUCCESS;
}

void RemoveTask::endInteractive(int error_code)
{
	switch(error_code)
	{
		case SUCCESS:
			parent->printInteractive("Song removed succesfully.", 1); break;
      	case OK_EXIT_CHAR:
			break;
      	case INVALID_ID:
			parent->printInteractive("Invalid -id passed as an argument...", 1); break;
      	case NO_ID:
			parent->printInteractive("No -id passed as an argument...", 1); break;
      	case DB_ERROR:
			parent->printInteractive("Could not remove a song due to internal database error.", 1); break;
      	case SONG_NOT_FOUND:
			parent->printInteractive("Could not find this song in database, hence can't remove...", 1); break;
		default:
			parent->printInteractive("Could not remove a song due to unknown error...", 1); break;
	}
}
