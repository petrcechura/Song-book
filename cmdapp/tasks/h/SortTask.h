#include <string>
#include <filesystem>
#include <iostream>
#include "cmdapputils.h"

class SongBookApp;

/** Sort a song inside database */
class SortTask : public Task<SongBookApp>
{
  public:
    SortTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};

    int Start() override;
};
