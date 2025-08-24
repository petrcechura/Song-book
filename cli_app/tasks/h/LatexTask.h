#include <string>
#include <filesystem>
#include "cmdapputils.h"
#include "json.hpp"

class SongBookApp;

class LatexTask : public Task<SongBookApp>
{
public:
  LatexTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

  int executeCommand() override;

  /** Replaces special charactes in string to printable alternatives. */
  std::string getPrintableString(std::string str);
  void setTexPath(std::string tex_path);
  void setOutPath(std::string out_path);

private:
  std::filesystem::path out_path = "database.pdf";
  std::filesystem::path tex_path = "database.tex";

};
