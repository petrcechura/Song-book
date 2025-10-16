#include <string>
#include <iostream>
#include "SongBookUtils.h"
#include "json.hpp"
#include <filesystem>

SongBookUtils* SongBookUtils::_utils = nullptr;
const char SongBookUtils::config_delimiter = '/';
nlohmann::json SongBookUtils::config = nullptr;
SongBookUtils* SongBookUtils::getInstance()
{
  if (_utils == nullptr)  {
    _utils = new SongBookUtils();
  }
  return _utils;
}

std::string SongBookUtils::getConfigItem(std::string _path)
{
  if (config != nullptr) {
    return parseConfigPath(config, _path);
  }
  else {
    return "";
  }
}

std::string SongBookUtils::parseConfigPath(nlohmann::json j, std::string _path)
{
  std::string item = "";
  bool del = false;
  int i = 0;
  for (; i < _path.size(); i++)  {
    if (_path[i] == config_delimiter)  {
      del = true;
      break;
    }
    item += _path[i];
  }
  _path.erase(0, i+1);
  if (j.contains(item))  {
    if (del)  {
      return parseConfigPath(j[item], _path);
    }
    else {
      std::string ret;
      try {
        // no other than string argument is allowed!
        ret = j[item].get<std::string>();
      }
      catch (const std::exception& e)  {
        ret = "";
      }
      return ret;
    } 
  }
  else {
    return "";
  }
}

// Aux. command that calls a system command and returns its output as an std::string 
std::string SongBookUtils::execSystemCommand(const char* cmd) 
{
    std::array<char, 1000> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool SongBookUtils::systemCommandExists(const char* cmd)
{
  std::string w = "which " + std::string(cmd) + " > /tmp/nul";
	if (system(w.c_str()))  {
		return false;
	}
  else {
    return true;
  }
}

std::string SongBookUtils::sql2txt(std::string sql)
{
  	// replace "\\n" with '\n'
		size_t pos = 0;
		while ((pos = sql.find("\\n", pos)) != std::string::npos) {
        	sql.replace(pos, 2, "\n");
        	pos += 1;
    }
    // replace "\\'" with '\''
    pos = 0;
		while ((pos = sql.find("\\'", pos)) != std::string::npos) {
        	sql.replace(pos, 2, "\'");
        	pos += 1;
    }

    return sql;
}

std::string SongBookUtils::txt2sql(std::string txt)
{

    // replace `"` with `\"`
    size_t pos = 0;
    while ((pos = txt.find("\"", pos)) != std::string::npos) {
        txt.replace(pos, 1, "\\\"");
        pos += 2;  // advance past the escaped quote
    }

    // replace `'` with `\'`
    pos = 0;
    while ((pos = txt.find("'", pos)) != std::string::npos) {
        txt.replace(pos, 1, "''");
        pos += 2;  // advance past the escaped single quote
    }

    return txt;
}
