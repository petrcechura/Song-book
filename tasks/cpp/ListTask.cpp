#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "../h/ListTask.h"
#include "../../SongBookApp.h"
#include "../../lib/json/json.hpp"

int ListTask::Start()
{ 
  using json_t = nlohmann::json;

	json_t data = parent->getDatabase()->getJson();
	

	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;
	std::cout << std::setw(4) << std::left << "ID"
			  << std::setw(NAME_WIDTH) << "Name "
			  << std::setw(AUTHOR_WIDTH) << "Author "
			  << std::endl;
	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;
	
	for(const auto& [key, item] : data.items())  {
    printSong(key.c_str(), item.at("name"), item.at("author"));
	}


	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;

	return 0;
}


void ListTask::printSong(const std::string& id, const std::string& name, const std::string& author)
{
  
	std::cout << std::setw(4) << std::left << id
				    << std::left << alignString(name, ' ', NAME_WIDTH)
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


