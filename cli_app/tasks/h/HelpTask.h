#include <string>
#include "cmdapputils.h"
class SongBookApp;
class HelpTask : public Task<SongBookApp>
{
public:
  HelpTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

  int startInteractive() override;
  int executeCommand() override;
};
