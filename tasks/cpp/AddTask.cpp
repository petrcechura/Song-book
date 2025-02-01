#include <string>
#include <iostream>
#include <iomanip>
#include "../h/AddTask.h"
#include "../../SongBookApp.h"


int AddTask::Start()
{
	std::string name;
	std::string author;
	
	std::cout << "Type a song name\n\t>>";
	std::getline(std::cin, name);
	if (name.size() > NAME_WIDTH)  {
		std::cout << "name '" << name << "' is too long! Max width is " 
				  << NAME_WIDTH << " chars, your's " << name.size() << std::endl;

		return 1;
	}
	else if (name == "")  {
		std::cout << "name field shall not be empty string!" << std::endl;
		return 2;
	}
	else if (name == std::string(1,parent->getExitChar()) )  {
		return 0;
	}
  else if (const Song* s = parent->getDatabase()->getSong(name); s != nullptr)  {
    std::cout << "Song with this name already exists!\n" << 
    s->getId() << " : " << s->getName() << " : " << s->getAuthor() << 
    "\n... you sure you want to proceed? (y/n)" << std::endl;
    
    std::string response;
	  std::getline(std::cin, response);
    
    if (response != "y")  {
        return 2;
    }
  }

	std::cout << "Type an author's name\n\t>>";
	std::getline(std::cin, author);

	if (author.size() > AUTHOR_WIDTH)  {
		std::cout << "author's name '" << author << "' is too long! Max width is " 
				  << AUTHOR_WIDTH << " chars, your's " << author.size() << std::endl;

		return 1;
	}
	else if (author == std::string(1,parent->getExitChar()) )  {
		return 0;
	}

	Song* song = new Song(name, author);
	parent->getDatabase()->addSong(song);

	std::cout << "Song '" << name << "' added..." << std::endl;

	return 0;
}
