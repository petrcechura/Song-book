#include <string>
#include <filesystem>
#include "WindowServer.h"
#include "SongBookUtils.h"
#include "Window.h"
#include "WTask.h"

class SongBookApp;

class ListTask : public WTask<SongBookApp>
{
public:
  ListTask(std::string name, SongBookApp* parent, std::string description) 
    : WTask<SongBookApp>(name, parent, description) { 
        upper_song_no = 0; 
        lower_song_no = MAX_ITEMS_PER_PAGE;
        upper_collection_no = 0; 
        lower_collection_no = MAX_ITEMS_PER_PAGE;
        select_song = 0;
        select_collection = 0;
      };
  virtual int Execute(char command) override;
  virtual int ParseCommand(std::string cmd_line) override { return 0; };
  void moveUp();
  void moveDown();
  void tableRight();
  void tableLeft();
  void listTable();
  void listSongs();
  void listCollections();

  std::string printSong(const nlohmann::json& song);
  std::string printSongListHeader();
  std::string printSongListBottom();
  std::string printCollectionListHeader();
  std::string printCollection(const nlohmann::json& collection);

private:
  int upper_song_no;
  int lower_song_no;
  int upper_collection_no;
  int lower_collection_no;
  int select_song;
  int select_collection;
  const int MAX_ITEMS_PER_PAGE = 17;
};
