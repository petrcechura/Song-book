#include <string>
#include <filesystem>
#include <iostream>
#include "../../lib/cmdapp/cmdapp.h"

class SongBookApp;

/** Remove a song inside a database */
class RemoveTask : public Task<SongBookApp>
{
  public:
    RemoveTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};

    int Start() override;
};
