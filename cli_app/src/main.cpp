#include "SongBookApp.h"
#include <map>

int main(int argc, char* argsv[])
{
  SongBookApp app = SongBookApp();


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

  // *************
  // argument -cmd
  // *************
  if (args.count("-cmd"))  {
    app.execCmd(args.at("-cmd"));
  }


  // *******************
  // argument -loop
  // *******************
  if (args.count("-loop"))  {
    if (args.at("-loop") == "true")  {
      app.startLoop();
    }
    else if (args.at("-loop") != "false")  {
      std::cout << "ERROR: Unknown option '" << args.at("-loop") << "' for '-loop'! Select true/false instead..." << std::endl;
    }
  }
  else {
    app.startLoop();
  }
}
 