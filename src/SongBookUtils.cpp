#include <string>
#include <iostream>
#include <sstream>
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

std::string SongBookUtils::printSong(const std::string& no,
                                     const std::string& name, 
                                     const std::string& author, 
                                     bool has_lyrics)
{
 
  std::ostringstream string_cont;
	string_cont << std::setw(4) << std::left << no
				      << std::left << alignString(name, 	' ', TITLE_WIDTH)
				      << std::left << alignString(author, ' ', ARTIST_WIDTH)
              << "    " << (has_lyrics ? "X" : " ");

  return string_cont.str().c_str();
}

std::string SongBookUtils::printSongListHeader()
{
  std::ostringstream string_cont;

	string_cont << std::setw(4) << std::left << "NO"
			        << std::setw(TITLE_WIDTH) << "Title "
			        << std::setw(ARTIST_WIDTH) << "Artist "
              << "Has lyrics?";

  return string_cont.str().c_str();
}

std::string SongBookUtils::printSongListBottom()
{
  std::ostringstream string_cont;
  string_cont << std::string(4 + 4 + TITLE_WIDTH + ARTIST_WIDTH+ 11, '-') << std::endl;

  return string_cont.str().c_str();
}

void SongBookUtils::printInteractive(const std::string& text, unsigned int indentation, bool newline)
{
  std::string t = "#";
  t += std::string(indentation, '#');
  
  std::cout << t << " " <<  text;
  if (newline) 
    std::cout << std::endl;
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

    pos = 0;
		while ((pos = sql.find("\\\"", pos)) != std::string::npos) {
        	sql.replace(pos, 2, "\"");
        	pos += 1;
    }

    
    // Replace each single quote ' with the sequence '\'' (used for shell escaping)
    pos = 0;
    const std::string from = "'";
    const std::string to   = "'\\''"; // C++ literal that produces the characters:  '  \  '  '
    while ((pos = sql.find(from, pos)) != std::string::npos) {
        sql.replace(pos, from.size(), to);
        pos += to.size(); // advance past the inserted text to avoid re-checking it
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

std::string SongBookUtils::convert_to_ascii(std::string str)  {
    std::map<wchar_t, char> cz_chars = {
        {L'Á', 'a'}, {L'á', 'a'},
        {L'Č', 'c'}, {L'č', 'c'},
        {L'Ď', 'd'}, {L'ď', 'd'},
        {L'É', 'e'}, {L'é', 'e'},
        {L'Ě', 'e'}, {L'ě', 'e'},
        {L'Í', 'i'}, {L'í', 'i'},
        {L'Ň', 'n'}, {L'ň', 'n'},
        {L'Ó', 'o'}, {L'ó', 'o'},
        {L'Ř', 'r'}, {L'ř', 'r'},
        {L'Š', 's'}, {L'š', 's'},
        {L'Ť', 't'}, {L'ť', 't'},
        {L'Ú', 'u'}, {L'ú', 'u'},
        {L'Ý', 'y'}, {L'ý', 'y'},
        {L'Ž', 'z'}, {L'ž', 'z'},
        {L'ů', 'u'}
    };
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wstr = converter.from_bytes(str);

    for (int i = 0; i < wstr.size(); i++)  {
      // convert cz characters to ascii
      if (cz_chars.count(wstr[i]))  {
        wstr[i] = cz_chars[wstr[i]];
      }
      
      // convert capitals into low letters
      if (wstr[i] > 64 && wstr[i] < 91)  {
        wstr[i] += 32; 
      }
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter2;
    return converter2.to_bytes(wstr);
} 

void SongBookUtils::printError(std::string msg)
{
    std::cerr << msg << std::endl;
}

/** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
int SongBookUtils::countStringChars(const std::string& _str)
{
  std::wstring str = std::wstring_convert<std::codecvt_utf8<wchar_t>>()
    .from_bytes(_str);
  return str.size();
}

/** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
std::string SongBookUtils::alignString(const std::string& _str, char fill, int maxWidth)
{	
  std::string str = "CANT DISPLAY (too long)" + std::string(maxWidth-23, fill);

  if (_str.size() < maxWidth)  {
    str = _str + std::string(TITLE_WIDTH - countStringChars(_str), fill);
  }

  return str;
}
