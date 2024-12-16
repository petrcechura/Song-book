#include "json/json.hpp"
#include <vector>
#include <map>
#include "Song.h"
#include <algorithm>
#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>
#include "Database.h"

Database::Database(std::string fname, std::string backupDir)  {
    this->song_container = std::map<int, Song*>();
    this->song_count = 0;
	  this->fname = fname;
    this->backupDir = backupDir;
}

Database::~Database()  {
    if (!this)
        delete this;
}


// TODO handle 'Ch' character somehow...
int Database::compare(std::string firstString, std::string secondString)  {
std::map<wchar_t, char> cz_chars = {
    {L'Á', 'a'}, {L'á', 'a'},
    {L'Č', 'c'}, {L'č', 'c'},
    {L'Ď', 'd'}, {L'ď', 'd'},
    {L'É', 'e'}, {L'é', 'e'},
    {L'Ě', 'e'}, {L'ě', 'e'},
    {L'Í', 'i'}, {L'í', 'i'},
    {L'Ň', 'n'}, {L'ň', 'n'},
    {L'Ó', 'o'}, {L'ó', 'o'},
    {L'Ř', 'r'}, {L'ř', 'r'},
    {L'Š', 's'}, {L'š', 's'},
    {L'Ť', 't'}, {L'ť', 't'},
    {L'Ú', 'u'}, {L'ú', 'u'},
    {L'Ý', 'y'}, {L'ý', 'y'},
    {L'Ž', 'z'}, {L'ž', 'z'},
    {L'ů', 'u'}
};
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring str1 = converter.from_bytes(firstString);
    std::wstring str2 = converter.from_bytes(secondString);

    for (int i = 0; i < str1.size(); i++)  {
      
      // convert czech characters to ascii
      if (cz_chars.count(str1[i]))  {
        std::cout << cz_chars[str1[i]] << " " << str1[i] << std::endl;
        str1[i] = cz_chars[str1[i]];
      }
      if (cz_chars.count(str2[i]))  {

        std::cout << cz_chars[str2[i]] << " " << str2[i] << std::endl;
        str2[i] = cz_chars[str2[i]];
      }
      
      // convert capitals into low letters
      if (str1[i] > 64 && str1[i] < 91)  {
        str1[i] += 32; 
      }
      if (str2[i] > 64 && str2[i] < 91)  {
        str2[i] += 32; 
      }

      if (str1[i] < str2[i])  {
        return -1;
      }
      else if (str1[i] > str2[i])  {
        return 1;
      }
    }

    return 0;
}


int Database::addSong(Song* song)  {

    if (song->getName() == "")  {
        // TODO
    }
    if (song->getAuthor() == "")  {
        // TODO
    }

    song->setId(song_count);
    this->song_container[song_count] = song;
    song_count++;

    return 0;
}

int Database::addSong(std::string json_string)  {
    return 1;
}

Database::json_t Database::getJson()  {

    Database::json_t j = json_t();

    for (auto song : song_container)  {
        Database::json_t _j  = song.second->getJson();
            // TODO throw an error, if smth went wrong
        j[song.first] = _j;
    }

    return j;
}

Song* Database::getSong(int id)
{
    if (auto search = song_container.find(id); search != song_container.end())  {
        return search->second;
    }
    else  {
		return nullptr;
    }
}


// bubble sort impl.
int Database::sort(std::string criteria)  
{
  
  if (criteria == "name")  {

    // sort by name
    for (int i = 0; i < song_container.size()-1; i++)  {
      for (int j = 0; j < song_container.size()-1; j++)  {
        int comp = compare(song_container[j]->getName(), song_container[j+1]->getName());
        if (comp > 0)  {
          auto _song = song_container[j];
          song_container[j] = song_container[j+1];
          song_container[j+1] = _song;
        }
        // if name matches, sort by author
        if (!comp)  {

          int comp2 = compare(song_container[j]->getAuthor(), song_container[j+1]->getAuthor());
          if (comp2 > 0)  {
            auto _song = song_container[j];
            song_container[j] = song_container[j+1];
            song_container[j+1] = _song;
          }
        }
      }
    } 
  }

  else if (criteria == "author")  {
    // sort by author
    for (int i = 0; i < song_container.size()-1; i++)  {
      for (int j = 0; j < song_container.size()-1; j++)  {
        int comp = compare(song_container[j]->getAuthor(), song_container[j+1]->getAuthor());
        if (comp > 0)  {
          auto _song = song_container[j];
          song_container[j] = song_container[j+1];
          song_container[j+1] = _song;
        }
        // if name matches, sort by name
        if (!comp)  {

          int comp2 = compare(song_container[j]->getName(), song_container[j+1]->getName());
          if (comp2 > 0)  {
            auto _song = song_container[j];
            song_container[j] = song_container[j+1];
            song_container[j+1] = _song;
          }
        }
      }
    } 
  }
  else {
    return 1;
  }

  return 0;
}

int Database::saveJsonFile()
{
	std::ofstream file(this->fname);
	
	file << getJsonString();

	file.close();

	return 0;

}

Database::json_t Database::findSong(std::string regex)
{
  Database::json_t json;
  int i = 0;
  std::regex const e{regex};
  std::smatch m;
  for (const auto& song : this->song_container)  {
    std::string txt = song.second->getName();
    

    if (std::regex_match(txt, m, e))  {
      json[i++] = txt;
    }
  }

  return json;

}

int Database::loadJsonFile(std::string fname)
{	
	std::ifstream file(fname);

	if (file.fail())  {
		return 1;
	}
	
	json_t data = json_t::parse(file);

	// TODO check proper format

	if (data.is_discarded())  {
		return 2;
	}

	for (auto &el : data.items())  {
		Song* song = new Song();
		song->loadJson(el.value());
		addSong(song);
	}
	
	return 0;

}


// TODO
int Database::removeSong(Song* song)
{
	//TODO
	return 1;
}


// TODO
int Database::removeSong(int id)
{
	int removed = song_container.erase(id);

	if (removed == 1)  {
		// remove was successfull
		return 0;
	}
	else if (removed == 0)  {
		// element with this ID does not exist!
		return 1;
	}
	else  {
		// more than one element removed! FATAL ERROR
		return 2;
	}

}


int Database::makeBackup()
{
  time_t timestamp;
  time(&timestamp);
  
  // TODO make it universal for OS
  std::string backup_name = this->backupDir + "/" + "backup-" + ctime(&timestamp);
  
  // if the `database file` does not exist, return 1
  if (!std::filesystem::exists(fname))  {
    return 1;
  }
  
  // if the backup dir does not exist, create it
  if (!std::filesystem::exists(backupDir))  {
    std::cout << "creating backup dir " << backupDir << " ...\n";
    std::filesystem::create_directories(backupDir);
  }

  std::filesystem::copy_file(fname, backup_name);

  return 0;

}
