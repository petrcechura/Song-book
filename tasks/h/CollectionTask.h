#include <string>
#include <filesystem>
#include "WindowServer.h"
#include "Window.h"
#include "WTask.h"

class SongBookApp;

class CollectionTask : public WTask<SongBookApp>
{
public:
  CollectionTask(std::string name, SongBookApp* parent, std::string description) 
    : WTask<SongBookApp>(name, parent, description) {};
  virtual int Execute(char command) override;
  virtual int ParseCommand(std::string cmd_line) override { return 0; };

  void selectCollection();
  void createCollection();
  void deleteCollection();

  void addToCollection();
  void removeFromCollection();

};
