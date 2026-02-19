#include <string>
#include <filesystem>
#include <iostream>
#include "cmdapputils.h"

class SongBookApp;


class ModifyTask : public WTask<SongBookApp>
{
public:
  ModifyTask(std::string name, SongBookApp* parent, std::string description) 
    : WTask<SongBookApp>(name, parent, description) { };

    // virtual functions override
    virtual int Execute(char command) override;

};
