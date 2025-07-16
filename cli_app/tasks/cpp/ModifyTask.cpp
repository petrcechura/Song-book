#include <string>
#include <iostream>
#include <iomanip>
#include "ModifyTask.h"
#include "SongBookApp.h"


int ModifyTask::Start(bool interactive)
{
	std::string str_id;
	std::cout << "Type an ID of song you want to modify\n\t>>";
	std::getline(std::cin, str_id);

	int id;

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return 0;
  	}

	try {
		id = std::stoi(str_id);
	}
	catch (const std::invalid_argument& e)  {
		std::cout << "'" << str_id << "' does not contain valid ID to parse!" << std::endl;
		return 1;
	}

	
	nlohmann::json song = parent->getDatabase()->getSong(id);
	
	if (song != "")  {
		std::cout << "++++ MODIFY ++++" << std::endl;
		std::cout << "NAME: " << song["TITLE"] << std::endl;
		std::cout << "AUTHOR: " << song["ARTIST"] << '\n' << std::endl;
		
		std::string name = "";
		std::string author = "";
		std::cout << "\tType new song name (leave blank for no modification)\n\t>>";
		std::getline(std::cin, name);
		std::cout << "\tType new author name (leave blank for no modification)\n\t>>";
		std::getline(std::cin, author);

		if (name != "")  {
			song["TITLE"] = name;
		}
		if (author != "")  {
			song["ARTIST"] = author;
		}

		int exit = parent->getDatabase()->addSong(song.dump(),true);
		
		if (!exit)  {
			std::cout << "Song " << song["TITLE"] << " has been modified succesfully..."  << std::endl;
		}
		else {
			std::cout << "Failed to modify " << song["TITLE"] << "..." << std::endl;
		}
		
		return exit;

	}
	else  {
		std::cout << "Song with ID " << id << " does not exist!" << std::endl;
		return 1;
	}
}
