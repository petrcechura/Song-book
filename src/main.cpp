#include "SongBookApp.h"
#include <map>
#include <fstream>
#include "json.hpp"
#include "SongBookUtils.h"
#include "WindowServer.h"
#include "Window.h"


int main(int argc, char* argsv[])
{
  // Parse command-line arguments
  std::map<std::string, std::string> args;
  std::string str = "";
  std::string arg = "";
  int char_cnt = 0;

  for (int idx = 1; idx != argc; idx++)  {

    // arguments start with '-' character, therefore create pair of such argument and empty string
    if (argsv[idx][0] == '-')  {
      arg = argsv[idx];
      args[arg] = "";
      char_cnt = 0;
      continue;
    }

    // trailing string without '-' characters are assigned with last obtained argument
    else if (arg != "") {
      if (char_cnt > 0)  {
        args[arg].append(" ");
      }
      args[arg].append(argsv[idx]);
      char_cnt++;
    }

    else {
      std::cout << "ERROR: Unknown arg '" << argsv[idx] << "'!" << std::endl;
    }
  }

  // ****************
  // argument -config
  // ****************

  nlohmann::json data;
  if (args.count("-config"))  {
    try {
      std::ifstream f(args.at("-config"));
      data = nlohmann::json::parse(f);
    }
    catch (const std::exception& e)  {
      std::cout << "Could not read file '" << args.at("-config") << "'" <<std::endl;
    }
  }
  
  SongBookUtils::getInstance()->setJson(data);
  
  SongBookApp* app = new SongBookApp();
  
  std::string error_log_path = SongBookUtils::getInstance()->getConfigItem("commons/error_file");
  if (error_log_path != "") {
    static std::ofstream error_file(error_log_path);
    std::cerr.rdbuf(error_file.rdbuf());
  }

  app->Init();
  app->Loop();

  
  // *************
  // argument -cmd
  // *************
  if (args.count("-cmd"))  {
    app->executeCommands(args.at("-cmd"));
  }
  else if (data.contains("commons") && data["commons"].contains("cmd"))  {
    app->executeCommands(data["commons"].at("cmd"));
  }

  // *******************
  // argument -loop
  // *******************
  if (args.count("-loop"))  {
    if (args.at("-loop") == "true")  {
      app->Loop();
    }
  }
  else if (data.contains("commons") && data["commons"].contains("loop"))  {
    if (data["commons"]["loop"] == true)  {
      app->Loop();
    }
  }
    
    delete app;
    
    return 0;
}
 