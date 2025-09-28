#include <string>
#include "cmdapputils.h"

class SongBookApp;

class BackupTask : public Task<SongBookApp>
{
public:
  BackupTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

    int executeCommand(int error_code) override;

};
