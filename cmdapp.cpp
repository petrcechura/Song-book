#include "argparse/argparse.hpp"
#include "json/json.hpp"
#include <exception>
#include <iostream>
#include <string>
#include "Database.h"
#include <iomanip>

#define NAME_WIDTH 30
#define AUTHOR_WIDTH 30
#define EXIT_STR "-"

using json_t = nlohmann::json;






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
					//TODO
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
