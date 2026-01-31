#include <string>
#include <filesystem>
#include "WindowServer.h"
#include "Window.h"
#include "WTask.h"

class SongBookApp;

class ListTask : public WTask<SongBookApp>
{
public:
  ListTask(std::string name, SongBookApp* parent, std::string description) 
    : WTask<SongBookApp>(name, parent, description) { up_no = 0; down_no = MAX_ITEMS_PER_PAGE; };
  virtual int Execute(char command) override;
  virtual int ParseCommand(std::string cmd_line) override { return 0; };
  void moveUp();
  void moveDown();
  void listSongs();

private:
  int select;
  int up_no;
  int down_no;
  const int MAX_ITEMS_PER_PAGE = 17;
  int current_id;
};
