#include <string>
#include "cmdapputils.h"

class SongBookApp;

class ExitTask : public Task<SongBookApp>
{
public:
  ExitTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

    int executeCommand(int error_code) override;

};
