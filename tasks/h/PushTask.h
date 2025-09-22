#include <string>
#include "cmdapputils.h"

class SongBookApp;

class PushTask : public Task<SongBookApp>
{
public:
  PushTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

    int executeCommand() override;

};
