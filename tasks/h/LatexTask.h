#include <string>
#include <filesystem>
#include "SongBookFormatter.h"
#include "json.hpp"
#include "WindowServer.h"
#include "Window.h"
#include "WTask.h"

class SongBookApp;

class LatexTask : public WTask<SongBookApp>
{
public:
  LatexTask(std::string name, SongBookApp* parent, std::string description) 
    : WTask<SongBookApp>(name, parent, description) { this->formatter = new BardFormatter(); };
  
  virtual int Execute(char command) override;

  /** Replaces special charactes in string to printable alternatives. */
  std::string getPrintableString(std::string str);
  void setOutPath(std::string out_path);

private:
  std::filesystem::path out_path = "database.pdf";
  SongBookFormatter* formatter;

  void exportToPdf();

};
