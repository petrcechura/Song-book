#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "ListTask.h"
#include "SongBookApp.h"
#include "json.hpp"

int ListTask::Start()
{ 
	nlohmann::json data = parent->getDatabase()->getJson();

	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;	
	std::cout << std::setw(4) << std::left << "ID"
			  << std::setw(NAME_WIDTH) << "Title "
			  << std::setw(AUTHOR_WIDTH) << "Artist "
			  << std::endl;
	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;

	std::cout << data.dump() << std::endl;
	
	for(const auto& [key, item] : data.items())  {
		std::string title = item.count("TITLE") ? item.at("TITLE") : "NULL";
		std::string artist = item.count("ARTIST") ? item.at("ARTIST") : "NULL";
		std::string id = item.count("ID") ? item.at("ID") : "NULL";
    	printSong(id, title, artist);
	}

	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;

	return 0;
}


void ListTask::printSong(const std::string& id, const std::string& name, const std::string& author)
{
  
	std::cout << std::setw(4) << std::left << id
				    << std::left << alignString(name, 	' ', NAME_WIDTH)
				    << std::left << alignString(author, ' ', AUTHOR_WIDTH)
				    << std::endl;
}

int ListTask::countStringChars(const std::string& _str)
{
  std::wstring str = std::wstring_convert<std::codecvt_utf8<wchar_t>>()
    .from_bytes(_str);
  return str.size();
}

std::string ListTask::alignString(const std::string& _str, char fill, int maxWidth)
{	
    std::string str = "CANT DISPLAY (too long)" + std::string(maxWidth-23, fill);

    if (_str.size() < maxWidth)  {
      str = _str + std::string(NAME_WIDTH - countStringChars(_str), fill);
    }

    return str;
}


