#include <string>
#include <iostream>
#include <iomanip>
#include "AddTask.h"
#include "SongBookApp.h"


int AddTask::Start()
{
	std::string name;
	std::string author;
	
	std::cout << "Type a song name\n\t>>";
	std::getline(std::cin, name);
	if (name.size() > TITLE_WIDTH)  {
		std::cout << "name '" << name << "' is too long! Max width is " 
				  << TITLE_WIDTH << " chars, your's " << name.size() << std::endl;

		return 1;
	}
	else if (name == "")  {
		std::cout << "name field shall not be empty string!" << std::endl;
		return 2;
	}
	else if (name == std::string(1,parent->getExitChar()) )  {
		return 0;
	}
  	else if (nlohmann::json song = parent->getDatabase()->getSong(name); song != nullptr)  {
    	std::cout << "Song with this name already exists!\n" << 
    	song["ID"] << " : " << song["TITLE"] << " : " << song["ARTIST"] << 
    	"\n... you sure you want to proceed? (y/n)" << std::endl;
    
    	std::string response;
	  	std::getline(std::cin, response);
    
    	if (response != "y")  {
        	return 2;
    	}
  	}

	std::cout << "Type an author's name\n\t>>";
	std::getline(std::cin, author);

	if (author.size() > ARTIST_WIDTH)  {
		std::cout << "author's name '" << author << "' is too long! Max width is " 
				  << ARTIST_WIDTH << " chars, your's " << author.size() << std::endl;

		return 1;
	}
	else if (author == std::string(1,parent->getExitChar()) )  {
		return 0;
	}

	nlohmann::json song = nlohmann::json();
	song["TITLE"] = name;
	song["ARTIST"] = author;
	
	int exit = parent->getDatabase()->addSong(song.dump());

	if (!exit)  {
		std::cout << "Song '" << name << "' added..." << std::endl;
	}
	else {
		std::cout << "Song '" << name << "' could not be added" << std::endl;
	}

	return 0;
}
