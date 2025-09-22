#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "RemoveTask.h"
#include "json.hpp"
#include "SongBookApp.h"

int RemoveTask::executeCommand()
{
	if (argumentExists("-id", true))  {
		std::string str_id = getArgument("-id").values[0];
		int id;
		
		try {
			id = std::stoi(str_id);
		}
		catch (const std::invalid_argument& e)  {
			return 1;
		}
		if (parent->getDatabase()->songExists(id))  {
 			return parent->getDatabase()->removeSong(id);
		}
		else {
			return 2;
		}

	}
	else {
		return 3;
	}

	return 1;
}

int RemoveTask::startInteractive()
{
	std::string str_id;
	int id;
	parent->printInteractive("Type an ID of song you wish to remove");
	str_id = parent->getInput(1);

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return 0;
  	}
	else {
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

	return 0;
}

void RemoveTask::endInteractive(int error_code)
{
	if (!error_code)  {
		parent->printInteractive("Song has been succesfully removed...", 1);
	}
	else if (error_code==2)  {
		parent->printInteractive("Song with this ID does not exist!", 1);
	}
	else if (error_code==1)  {
		parent->printInteractive("Error processing -id as argument...", 1);
	}
}
