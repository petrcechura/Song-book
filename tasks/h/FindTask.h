#include <string>
#include "cmdapputils.h"

class SongBookApp;

class FindTask : public Task<SongBookApp>
{
public:
  FindTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

  int startInteractive() override;
  int executeCommand(int error_code) override;

  enum {
    SUCCESS,
    SONG_NOT_FOUND
  } ErrorCode;

};
