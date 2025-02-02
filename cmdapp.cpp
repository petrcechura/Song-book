

// ********************
// * IMPORTANT NOTICE *
// ********************
// This is an obsolete piece of code is no longer used!
// The only reason to keep it there is to have an information
// what the previous implementation had so it can be added
// to the newest one, too.
//
// It will be removed soon...


#include "argparse/argparse.hpp"
#include "json/json.hpp"
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include "Database.h"
#include <iomanip>

#define NAME_WIDTH 30
#define AUTHOR_WIDTH 30
#define EXIT_STR "-"

using json_t = nlohmann::json;

/** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30)
{	
	std::string str = "CANT DISPLAY (too long)" + std::string(maxWidth-23, fill);
	
	if (_str.size() < maxWidth)  {
		str = _str + std::string(NAME_WIDTH - countStringChars(_str), fill);
	}

	return str;
}

int add(Database* database)
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
	else if (name == EXIT_STR)  {
		return 0;
	}
  else if (const Song* s = database->getSong(name); s != nullptr)  {
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
	else if (author == EXIT_STR)  {
		return 0;
	}

	Song* song = new Song(name, author);
	database->addSong(song);

	std::cout << "Song '" << name << "' added..." << std::endl;

	return 0;
}

void sort(Database* database)
{
  std::string criteria;

  std::cout << "Type a sorting criteria\n\t>>";
  std::getline(std::cin, criteria);

  int i = database->sort(criteria);

  if (!i)  {
    std::cout << "Sort made succesfully..." << std::endl;
  }  
  else  {
    std::cout << "Unavailable sort criteria..." << std::endl;
  }
}

void find(Database* database)
{
  std::string regex;
  json_t data;

  std::cout << "Type a pattern to find\n\t>>";
  std::getline(std::cin, regex);
  
  data = database->findSong(regex);

  std::cout << "Matched songs:" << std::endl;
	for(const auto& [key, item] : data.items())  {
    std::cout << key << " - " << item << std::endl;
	}
}

int remove(Database* database)
{	
	std::string str_id;
	std::cout << "Type a song ID to remove\n\t>> ";
	std::getline(std::cin, str_id);
	
	int id;

	try {
		id = std::stoi(str_id);
	}
	catch (const std::invalid_argument& e)  {
		std::cout << "'" << str_id << "' does not contain valid ID to parse!" << std::endl;
		return 1;
	}
	
	
	const Song* song = database->getSong(id);

	if (song)  {
		std::string name = song->getName();
		int rm_success = database->removeSong(id);

		if (!rm_success) {
			std::cout << "Song '" << name << "' removed succesfully" << std::endl;
			return 0;
		}
		else if (rm_success == 1 )  {
			// element does not exist, cant be removed
			return 1;
		}
		else  {
			// FATAL ERROR (multiple elements with same id)
			return 2;
		}
	}	
	else  {
		std::cout << "Invalid song id '" << id << "' ..." << std::endl;
		return 1;
	}
}

int help()
{
	std::cout << "Available commands:" << std::endl;
	std::cout << "> help: Shows this message" << std::endl;
	std::cout << "> add:  Adds a new song to database" << std::endl;
	std::cout << "> remove: Removes a song from database" << std::endl;
	std::cout << "> list: Shows all songs from database" << std::endl;
	std::cout << "> modify: Modifies songs in database" << std::endl;
	std::cout << "> latex: Exports database to a LaTeX file" << std::endl;
	std::cout << "> exit: Saves database into json file and exits program" << std::endl;
  std::cout << "> backup: Saves database into json file and makes a backup (saving current database into separate directory)" << std::endl;
  std::cout << "> sort: Sorts database by chosen criteria (available criteria: 'name', 'author')" << std::endl;
  std::cout << "> find: Tries to find a song matching given Regular Expression" << std::endl;

	return 0;
}

void printSong(const std::string& id, const std::string& name, const std::string& author)
{
  
	std::cout << std::setw(4) << std::left << id
				    << std::left << alignString(name, ' ', NAME_WIDTH)
				    << std::left << alignString(author, ' ', AUTHOR_WIDTH)
				    << std::endl;
}

int list(Database* database)
{
	json_t data = database->getJson();

  std::cout << "JSON: " << data << std::endl;
	

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

int modify(Database* database)
{
	std::string str_id;
	std::cout << "Type an ID of song you want to modify\n\t>>";
	std::getline(std::cin, str_id);

	int id;

  if (str_id == EXIT_STR)  {
    return 0;
  }

	try {
		id = std::stoi(str_id);
	}
	catch (const std::invalid_argument& e)  {
		std::cout << "'" << str_id << "' does not contain valid ID to parse!" << std::endl;
		return 1;
	}

	
	Song* song = database->getSong(id);
	
	if (song)  {
		std::cout << "++++ MODIFY ++++" << std::endl;
		std::cout << "NAME: " << song->getName() << std::endl;
		std::cout << "AUTHOR: " << song->getAuthor() << '\n' << std::endl;
		
		std::string name = "";
		std::string author = "";
		std::cout << "\tType new song name (leave blank for no modification)\n\t>>";
		std::getline(std::cin, name);
		std::cout << "\tType new author name (leave blank for no modification)\n\t>>";
		std::getline(std::cin, author);

		if (name != "")  {
			song->setName(name);
		}
		if (author != "")  {
			song->setAuthor(author);
		}

		std::cout << "Song " << song->getName() << " has been modified succesfully..."  << std::endl;
		
		return 0;

	}
	else  {
		std::cout << "Song with ID " << id << " does not exist!" << std::endl;
		return 1;
	}


}

int latex(Database* database)
{
  const int SONGS_PER_PAGE = 25;

  const char* title_page = R"(
    \maketitle
  )";

  const char* header = R"(
    \documentclass[17pt]{memoir}
    \usepackage[czech]{babel}
    \usepackage{bookman}
    \usepackage[none]{hyphenat}
    \usepackage[T1]{fontenc}
    \fontfamily{georgia}\selectfont
    \usepackage{csvsimple}
    \pagenumbering{gobble}
    \usepackage{booktabs}    
    \usepackage{titling}
    \usepackage[utf8]{inputenc}  
    \usepackage{calc}
    \usepackage{layout}
    \usepackage{graphicx}  
    \usepackage{amsmath, amssymb} 
    \usepackage{ marvosym }
    \usepackage{threeparttable}

    % Define custom title page
    \pretitle{\begin{center}\Huge\bfseries} % Format title
    \posttitle{\par\end{center}}
    \preauthor{\begin{center}\Large\itshape} % Format author
    \postauthor{\par\end{center}}
    \predate{\begin{center}\normalsize} % Format date
    \postdate{\par\end{center}}

    \title{Písničkovník}
    \author{Iči}
    
    \begin{document})";

  const char* table_begin = R"(
    \begin{table}[htbp]
    \centering
    \begin{tabular}{p{1.2cm}|p{8.5cm}|p{6cm}}
    \toprule
    \textbf{ID} & \textbf{Název písně} & \textbf{Autor písně} \\
    \midrule)";

  const char* table_end = R"(
    \bottomrule
    \end{tabular} 
    \end{table} 
    \pagebreak)";
  
  const char* ending = R"(
    \end{document})";

  std::ofstream file("database.tex");

  file << header;
  file << title_page;
  file << table_begin;

	json_t data = database->getJson();
  for (int i = 0; i < database->getSongCount(); i++)  {

    const auto& song = database->getSong(i);
    file << i << " & " << song->getName(true) << " & " << song->getAuthor(true) << R"( \\)";

    if (i % SONGS_PER_PAGE == 0 && i > 0 && i < database->getSongCount()-1)  {
      file << table_end;
      file << table_begin;
    }
  }
  file << table_end;
  file << ending;
	
	file.close();

  system("pdflatex database.tex");

	return 0;
}

int main(int argc, char *argv[])
{	
	std::string user_input = "";

	argparse::ArgumentParser prog("songbook");
	// TODO
	prog.add_argument("-a");
	prog.add_argument("-c")
		.default_value(false)
		.implicit_value(true)
		.nargs(0)
		.help("when passed, app won't launch into CLI, instead just parses arguments and exits");
	prog.add_argument("--no-json-load") // TODO implement
		.default_value(false)
		.implicit_value(true)
		.help("when true, json file is not loaded into database");
	prog.add_argument("-do")
		.help("provide a -do command that is executed after app launch");
	prog.add_argument("-h", "--help")
		.default_value(false)
		.implicit_value(true)
		.nargs(0)
		.help("Displays a help message");

	try {
  		prog.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
  		std::cerr << err.what() << std::endl;
  		std::cerr << prog;
  		std::exit(1);
	}


	// create the database object
	Database* database = new Database;

	if (prog.get<bool>("--no-json-load") == false)  {
		if (database->loadJsonFile("database.json"))  {
			std::cout << "Error while trying to load 'database.json' file (It's either corrupted or does not exist)!" << std::endl;
		}
	}


	if (prog.get<bool>("-h") == true)  {
		std::cout << "TODO" << std::endl;
		exit(0);
	}

	// check arguments, do their job
	if (auto fn= prog.present("-do"))  {
		user_input = prog.get<std::string>("-do");
	}


	if (prog.get<bool>("-c") == true)  {
		exit(0);
	}
	else  {
		while(true)  {
			
			// get the input from user
			if (user_input == "")  {
				std::cout << ">> ";
				std::getline(std::cin, user_input);
			}

			if (user_input == "help")  {
					help();
			}
			else if (user_input == "add")  {
					add(database);
			}
			else if (user_input == "remove")  {
					remove(database);
			}
			else if (user_input == "modify")  {
					modify(database);
			}
			else if (user_input == "list")  {
					list(database);
			}
			else if (user_input == "latex")  {
          latex(database);
			}
      else if (user_input == "sort")  {
          sort(database);
      }
      else if (user_input == "find")  {
          find(database);
      }
      else if (user_input == "backup")  {
          database->saveJsonFile();

          int i = database->makeBackup();
          if (!i)  {
            std::cout << "backup file has been created..." << std::endl;
          }
          else  {
            std::cout << "error while creating backup file..." << std::endl;
          }
      }
			else if (user_input == "exit")  {
				database->saveJsonFile();
				std::cout << "exiting program..." << std::endl;
				exit(0);
			}
			else if (user_input == "")  {
				// nothing
			}
			else  {
				std::cout << "invalid input: " << user_input << std::endl;
			}


			user_input = "";
		}
	}
}
