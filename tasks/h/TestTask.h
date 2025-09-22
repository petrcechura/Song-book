#include <string>
#include "cmdapputils.h"

class SongBookApp;

class TestTask : public Task<SongBookApp>
{
public:
  TestTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

    int startInteractive() override;

};
