#include "argparse/argparse.hpp"
#include "json/json.hpp"
#include <exception>
#include <iostream>
#include <string>
#include "Database.h"
#include <iomanip>

#define NAME_WIDTH 30
#define AUTHOR_WIDTH 30

using json_t = nlohmann::json;

int add(Database* database)
{	
	std::string name;
	std::string author;
	
	std::cout << "Type a song name\n\t>>";
	std::getline(std::cin, name);

	std::cout << "Type an author's name\n\t>>";
	std::getline(std::cin, author);

	// TODO check validity

	Song* song = new Song(name, author);
	database->addSong(song);

	std::cout << "Song '" << name << "' added..." << std::endl;

	return 0;
}

int remove(Database* database)
{	
	std::string str_id;
	std::cout << "Type a song ID to remove\n\t>> ";
	std::getline(std::cin, str_id);

	int id = std::stoi(str_id);
	Song* song = database->getSong(id);

	// TODO: remove method shall be implemented, which is not currently...
	// and maybe remove it by id, not by pointer (safer option)
	if (song)  {
		std::string name = song->getName();
		database->removeSong(song);
		std::cout << "Song '" << name << "' removed succesfully" << std::endl;
		return 0;
	}	
	else  {
		std::cout << "Invalid song id '" << id << "' ..." << std::endl;
		return 1;
	}
}

int help()
{
	std::cout << "TODO" << std::endl;

	return 0;
}


// TODO does not work well with unicode characters...
int list(Database* database)
{
	json_t data = database->getJson();
	

	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;
	std::cout << std::setw(4) << std::left << "No. "
			  << std::setw(NAME_WIDTH) << "Name "
			  << std::setw(AUTHOR_WIDTH) << "Author "
			  << std::endl;
	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;
	int i = 1;

	for(auto& item : data)  {
		std::cout << std::setw(4) << std::left << std::to_string(i).append(". ")
				  << std::setw(NAME_WIDTH) << std::left << std::string(item.at("name"))
				  << std::setw(AUTHOR_WIDTH) << std::left << std::string(item.at("author"))
				  << std::endl;
		i++;
	}


	std::cout << std::string(4 + NAME_WIDTH + AUTHOR_WIDTH, '+')
			  << std::endl;


	return 0;
}

int latex()
{
	std::cout << "TODO" << std::endl;

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
	if (!database->loadJsonFile("database.json"))  {
		std::cout << "Error while trying to load 'database.json' file! Terminating..." << std::endl;
		exit(1);
	}


	// TODO
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
			else if (user_input == "list")  {
					list(database);
			}
			else if (user_input == "latex")  {
					//TODO
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
