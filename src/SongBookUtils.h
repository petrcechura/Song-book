#include <string>
#include <iostream>
#include "SongDatabase.h"
#include "SongBookFormatter.h"
#include "json.hpp"

#ifndef SONGBOOKUTILS_H
#define SONGBOOKUTILS_H

class SongBookUtils
{
  public:
    static SongBookUtils* getInstance();

    static std::string getConfigItem(std::string _path);
    static int setJson(nlohmann::json _json) { config = _json; return 0; };
    static nlohmann::json getJson() {return config; };
    // Aux. command that calls a system command and returns its output as an std::string 
    static std::string execSystemCommand(const char* cmd);
    /** Checks if a system command exists on this machine */
    static bool systemCommandExists(const char* cmd);

    /** Converts a piece of text from format that is stored inside sql (preserving newlines and other
        special character) into readable syntax */
    static std::string sql2txt(std::string sql);

  private:
    static nlohmann::json config;
    static SongBookUtils* _utils;
    SongBookUtils() {};
    static const char config_delimiter;

    static std::string parseConfigPath(nlohmann::json j, std::string _path);
};


#endif
