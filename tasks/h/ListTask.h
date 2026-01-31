#include <string>
#include <filesystem>
#include "WindowServer.h"
#include "Window.h"
#include "WTask.h"

class SongBookApp;

class ListTask : public WTask<WindowServer>
{
public:
    ListTask(std::string name, WindowServer* parent, std::string description) 
      : WTask<WindowServer>(name, parent, description) {};

  void printSong(const std::string& id, const std::string& name, const std::string& author);
  
  /** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
  inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30);

  /** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
  inline int countStringChars(const std::string& _str);

  virtual int executeCommand(char command) override;

};
