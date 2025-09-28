#include <string>
#include <filesystem>
#include <iostream>
#include "cmdapputils.h"

class SongBookApp;

/** Adds a song to a database */
class AddTask : public Task<SongBookApp>
{

  public:
    AddTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};
  
    const int NAME_WIDTH = 30;
    const int AUTHOR_WIDTH = 30;

    int startInteractive() override;
    int executeCommand(int error_code) override;
    void endInteractive(int error_code) override;

  private:
    enum {
      SUCCESS,
      OK_EXIT_CHAR,
      ERR_TITLE_TOO_LONG,
      ERR_TITLE_FIELD_EMPTY,
      ERR_SONG_EXISTS,
      ERR_ARTIST_TOO_LONG,
      ERR_SONG_NOT_ADDED,
      ERR_ADD_SONG_FAILED,
      ERR_MISSING_ARGS

    } ErrorCode;
};
