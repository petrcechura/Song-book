#include <string>
#include "cmdapputils.h"
#include "json.hpp"

class SongBookApp;

class LatexTask : public Task<SongBookApp>
{
public:
  LatexTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

int Start(bool interactive) override;

};
