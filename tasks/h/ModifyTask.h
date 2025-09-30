#include <string>
#include <filesystem>
#include <iostream>
#include "cmdapputils.h"

class SongBookApp;


/** Modify a song inside a database */
class ModifyTask : public Task<SongBookApp>
{
  public:
    ModifyTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};
 
    int startInteractive() override;
    int executeCommand(int error_code) override;
    void endInteractive(int error_code) override;


  private:
    enum {
      SUCCESS,
      OK_EXIT_CHAR,
      INVALID_ID,
      SONG_NOT_FOUND,
      NO_ID,
      MISSING_ARGS,
      DB_ERROR
    } ErrorCode;
};
