#include <string>
#include <filesystem>
#include <iostream>
#include "cmdapputils.h"

class SongBookApp;

/** Remove a song inside a database */
class RemoveTask : public Task<SongBookApp>
{
  public:
    RemoveTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};

    int Start(bool interactive) override;
};
