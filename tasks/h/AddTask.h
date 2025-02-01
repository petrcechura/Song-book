#include <string>
#include <filesystem>
#include <iostream>
#include "../../lib/cmdapp/cmdapp.h"

class SongBookApp;

/** Adds a song to a database */
class AddTask : public Task<SongBookApp>
{

  public:
    AddTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};
  
    const int NAME_WIDTH = 30;
    const int AUTHOR_WIDTH = 30;

    int Start() override;
};
