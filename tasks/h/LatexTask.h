#include <string>
#include "../../lib/cmdapp/cmdapp.h"
#include "../../lib/json/json.hpp"

class SongBookApp;

class LatexTask : public Task<SongBookApp>
{
public:
  LatexTask(std::string cmd, SongBookApp* parent) 
    : Task<SongBookApp>(cmd, parent) {};

int Start() override;

};
