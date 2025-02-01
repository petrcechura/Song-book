#include "lib/json/json.hpp"
#include <string>
#include <iostream>
#include "Song.h"
#include <set>


Song::Song(std::string name, std::string author)  {
    this->setName(name);
    this->setAuthor(author);
}


Song::~Song()  {
    if (!this)
        delete this;
}

Song::json_t Song::getJson() const {
    json_t j;

    j["name"] = getName();
    j["author"] = getAuthor();
    
    json_t _j = json_t();
    int i = 0;
    for (auto const& flag : flags)  {
        _j[i] = flag;
    }

    j["flags"] = _j;

    return j;
}


void Song::setId(int id)  {
    this->id = id;
}
void Song::setName(std::string name)  {
    this->name = name;
}
void Song::setAuthor(std::string author)  {
    this->author = author;
}

void Song::setFlag(std::string flag)  {
    if (flag != "")  {
        this->flags.insert(flag);
    }
}

void Song::setFlags(json_t flags)  {
    for (const auto& flag : flags)  {
        this->flags.insert(flag);
    }
}

int Song::deleteFlag(std::string flag)  {
    this->flags.erase(this->flags.find(flag));
    return 0;
}

void Song::loadJson(json_t j)
{	
	for (auto &el : j.items())  {
		if (el.key() == "name")  {
			this->setName(el.value());
		}
		else if (el.key() == "author")  {
			this->setAuthor(el.value());
		}
    else if (el.key() == "flags")  {
      this->setFlags(el.value());
    }
		else  {
			// TODO
		}
	}

}

inline void Song::replace(std::string& str, const std::string& subj, const std::string& replace_by) const
{
    size_t pos = str.find(subj);

    // Iterate through the string and replace all
    // occurrences
    while (pos != std::string::npos) {
        // Replace the substring with the specified string
        str.replace(pos, subj.size(), replace_by);

        // Find the next occurrence of the substring
        pos = str.find(subj, pos + replace_by.size());
    }
}



int Song::getId(bool latexFormat) const
{
  if (!latexFormat)  {
    return id;
  }
  else  {
    return id; // probably thats it...
  }
}

std::string Song::getName(bool latexFormat) const
{
  if (!latexFormat)  {
    return name;
  }
  else  {
    std::string lname = name;
    replace(lname, "&", "\\&");

    return lname;
  }
}

std::string Song::getAuthor(bool latexFormat) const
{
  if (!latexFormat)  {
    return author;
  }
  else  {
    std::string lauthor = author;
    replace(lauthor, "&", "\\&");

    return lauthor;
  }
}
