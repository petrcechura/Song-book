#include "../lib/json/json.hpp"
#include <map>
#include <vector>
#include "Song.h"
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>
#include "Database.h"

Database::Database(std::string fname, std::string backupDir)  {
    this->song_container = std::vector<Song*>();
	  this->fname = fname;
    this->backupDir = backupDir;
}

Database::~Database()  {
    if (!this)
        delete this;
}

inline std::string convert_to_ascii(std::string str)  {
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
    std::wstring wstr = converter.from_bytes(str);

    for (int i = 0; i < wstr.size(); i++)  {
      // convert cz characters to ascii
      if (cz_chars.count(wstr[i]))  {
        wstr[i] = cz_chars[wstr[i]];
      }
      
      // convert capitals into low letters
      if (wstr[i] > 64 && wstr[i] < 91)  {
        wstr[i] += 32; 
      }
    }

  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter2;
  return converter2.to_bytes(wstr);
} 


int Database::compare(std::string firstString, std::string secondString)  {

    std::string str1 = convert_to_ascii(firstString);
    std::string str2 = convert_to_ascii(secondString);


    for (int i = 0; i < str1.size(); i++)  {
      
      // tranform 'ch' character to 'h'+1 (i)
      if (str1[i] == 'c' && i < str1.size()-1)  {
        if (str1[i+1] == 'h')  {
          str1[i] = 'h' + 1;
        }
      }
      if (str2[i] == 'c' && i < str2.size()-1)  {
        if (str2[i+1] == 'h')  {
          str2[i] = 'h' + 1;
        }
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

    song->setId(song_container.size());
    this->song_container.push_back(song);

    return 0;
}

int Database::addSong(std::string json_string)  {
    return 1;
}

Database::json_t Database::getJson()  {

    Database::json_t j = json_t();

    
    int i = 0;
    for (const auto& song : song_container)  {

        Database::json_t _j  = song->getJson();
            // TODO throw an error, if smth went wrong
        j[i++] = _j;
    }

    return j;
}

Song* Database::getSong(int id)
{
  if (id >= song_container.size())  {
    return nullptr;
  }
  else  {
    return song_container[id];
  }
}

Song* Database::getSong(std::string name)
{
    for (auto &s : this->song_container)  {
        if (s->getName() == name)  {
            return s;
        }
    }
    return nullptr;
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

Database::json_t Database::findSong(std::string pattern)
{
  pattern = convert_to_ascii(pattern);

  json_t json;
  for (const auto& song : this->song_container)  {
    std::string txt = convert_to_ascii(song->getName());

    if (txt.find(pattern) != std::string::npos)  {
      json[std::to_string(song->getId())] = song->getName();

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
  if (id < song_container.size() && id >= 0)  {
	  song_container.erase(song_container.begin()+id);
    return 0;
  }

  else  {
    return 1;
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
