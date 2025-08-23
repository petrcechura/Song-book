#include <string>
#include "cmdapputils.h"

class SongBookApp;

class FetchTask : public Task<SongBookApp>
{
public:
  FetchTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

  int startInteractive() override;

};
