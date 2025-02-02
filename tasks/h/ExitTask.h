#include <string>
#include "../../lib/cmdapp/cmdapp.h"

class SongBookApp;

class ExitTask : public Task<SongBookApp>
{
public:
  ExitTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

int Start() override;

};
