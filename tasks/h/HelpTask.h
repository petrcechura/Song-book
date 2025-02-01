#include <string>
#include "../../lib/cmdapp/cmdapp.h"
class SongBookApp;
class HelpTask : public Task<SongBookApp>
{
public:
  HelpTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

int Start() override;
};
