#include <string>
#include "../../lib/cmdapp/cmdapp.h"

class SongBookApp;

class BackupTask : public Task<SongBookApp>
{
public:
  BackupTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

int Start() override;

};
