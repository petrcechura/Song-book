#include <string>
#include <filesystem>
#include "cmdapputils.h"

class SongBookApp;

class ListTask : public Task<SongBookApp>
{
public:
  ListTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};


  void printSong(const std::string& id, const std::string& name, const std::string& author);
  
  /** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
  inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30);

  /** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
  inline int countStringChars(const std::string& _str);

  const int NAME_WIDTH = 30;
  const int AUTHOR_WIDTH = 30;

  int Start() override;

};
