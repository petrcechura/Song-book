#include <string>
#include <filesystem>
#include "cmdapputils.h"
#include "SongBookFormatter.h"
#include "json.hpp"
#include <map>

class SongBookApp;

class DumpTask : public Task<SongBookApp>
{
public:
  DumpTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {

      // init formatters
      this->formatters["bard"] = new BardFormatter();
  };

  int executeCommand(int error_code) override;
  int startInteractive() override;
  void endInteractive(int error_code) override;


  /** Replaces special charactes in string to printable alternatives. */
  std::string getPrintableString(std::string str);
  void setOutPath(std::string out_path);

private:
  std::filesystem::path out_path = "database.pdf";

  std::map<std::string, SongBookFormatter*> formatters;

  enum {
    SUCCESS,
    GENERATE_SONGBOOK_FAILED,
    UNSUPPORTED_FORMAT
  } ErrorCode;

};
