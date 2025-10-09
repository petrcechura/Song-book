#include <string>
#include <filesystem>
#include "cmdapputils.h"
#include "SongBookFormatter.h"
#include "json.hpp"

class SongBookApp;

class LatexTask : public Task<SongBookApp>
{
public:
  LatexTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {
      this->formatter = new BardFormatter();
  };

  int executeCommand(int error_code) override;

  /** Replaces special charactes in string to printable alternatives. */
  std::string getPrintableString(std::string str);
  void setOutPath(std::string out_path);

private:
  std::filesystem::path out_path = "database.pdf";
  SongBookFormatter* formatter;

};
