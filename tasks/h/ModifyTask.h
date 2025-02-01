#include <string>
#include <filesystem>
#include <iostream>
#include "../../lib/cmdapp/cmdapp.h"

class SongBookApp;


/** Modify a song inside a database */
class ModifyTask : public Task<SongBookApp>
{
  public:
    ModifyTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};
 
    int Start() override;
};
