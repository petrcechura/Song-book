#include <string>
#include <iostream>
#include <sstream>
#include <format>
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

std::string SongBookUtils::getConfigItem(std::string path, std::string def)
{
    if (config.is_null())
        return def;

    nlohmann::json* current = &config;

    size_t start = 0;
    size_t end = path.find(config_delimiter);

    while (end != std::string::npos)
    {
        std::string key = path.substr(start, end - start);

        if (!current->contains(key))
            return def;

        current = &((*current)[key]);

        start = end + 1;
        end = path.find(config_delimiter, start);
    }

    std::string finalKey = path.substr(start);

    if (current->contains(finalKey) && (*current)[finalKey].is_string())
        return (*current)[finalKey].get<std::string>();

    return def;
}

nlohmann::json SongBookUtils::getConfigJson(std::string path, nlohmann::json def)
{
    if (config.is_null())
        return def;

    nlohmann::json* current = &config;
    if (current == nullptr)
      return def;

    size_t start = 0;
    size_t end = path.find(config_delimiter);

    while (end != std::string::npos)
    {
        std::string key = path.substr(start, end - start);

        if (!current->contains(key))
            return def;

        current = &((*current)[key]);

        start = end + 1;
        end = path.find(config_delimiter, start);
    }

    std::string finalKey = path.substr(start);

    if (current->contains(finalKey))  {
        // copy the object
        nlohmann::json ret((*current)[finalKey]);
        return ret;
    }

    return def;
}

void SongBookUtils::setConfigItem(std::string path, std::string value)
{
    if (config.is_null())
        return;

    nlohmann::json* current = &config;

    size_t start = 0;
    size_t end = path.find(config_delimiter);

    while (end != std::string::npos)
    {
        std::string key = path.substr(start, end - start);

        // Create object if it doesn't exist
        if (!current->contains(key) || !(*current)[key].is_object())
        {
            (*current)[key] = nlohmann::json::object();
        }

        current = &((*current)[key]);

        start = end + 1;
        end = path.find(config_delimiter, start);
    }

    // Final key
    std::string finalKey = path.substr(start);
    (*current)[finalKey] = value;
}

void SongBookUtils::setConfigJson(std::string path, nlohmann::json value)
{
    if (config.is_null())
        return;

    nlohmann::json* current = &config;

    size_t start = 0;
    size_t end = path.find(config_delimiter);

    while (end != std::string::npos)
    {
        std::string key = path.substr(start, end - start);

        // Create object if it doesn't exist
        if (!current->contains(key) || !(*current)[key].is_object())
        {
            (*current)[key] = nlohmann::json::object();
        }

        current = &((*current)[key]);

        start = end + 1;
        end = path.find(config_delimiter, start);
    }

    // Final key
    std::string finalKey = path.substr(start);
    (*current)[finalKey] = value;
}

void SongBookUtils::printInteractive(const std::string& text, unsigned int indentation, bool newline)
{
  std::string t = "#";
  t += std::string(indentation, '#');
  
  std::cout << t << " " <<  text;
  if (newline) 
    std::cout << std::endl;
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
    int count = 0;
    size_t i = 0;
    const size_t len = _str.size();

    while (i < len)
    {
        unsigned char c = static_cast<unsigned char>(_str[i]);
        size_t charLen;

        if ((c & 0x80) == 0x00)       // 0xxxxxxx -> 1-byte ASCII
            charLen = 1;
        else if ((c & 0xE0) == 0xC0)  // 110xxxxx -> 2-byte sequence
            charLen = 2;
        else if ((c & 0xF0) == 0xE0)  // 1110xxxx -> 3-byte sequence
            charLen = 3;
        else if ((c & 0xF8) == 0xF0)  // 11110xxx -> 4-byte sequence
            charLen = 4;
        else
            charLen = 1;               // invalid leading byte, skip just this byte

        // Guard against a truncated/invalid sequence running past the end
        // or containing bytes that aren't valid continuation bytes.
        size_t j = 1;
        for (; j < charLen && (i + j) < len; ++j)
        {
            unsigned char cc = static_cast<unsigned char>(_str[i + j]);
            if ((cc & 0xC0) != 0x80)  // not a continuation byte -> sequence is broken
                break;
        }

        i += (j < charLen) ? 1 : charLen; // if broken, only advance by 1 byte
        ++count;
    }

    return count;
}

/** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
std::string SongBookUtils::alignString(const std::string& _str, char fill, int maxWidth)
{
    int charCount = countStringChars(_str);

    if (charCount >= maxWidth)
    {
        // Not enough room to display the full string
        return "CANT DISPLAY (too long)" + std::string(std::max(0, maxWidth - 23), fill);
    }

    return _str + std::string(maxWidth - charCount, fill);
}

/** Replaces any invalid or truncated UTF-8 byte sequences with '?'. */
std::string SongBookUtils::sanitizeUtf8(const std::string& input)
{
    std::string out;
    out.reserve(input.size());
    size_t i = 0;
    const size_t len = input.size();

    while (i < len)
    {
        unsigned char c = static_cast<unsigned char>(input[i]);
        size_t charLen;

        if ((c & 0x80) == 0x00)      charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        else { out += '?'; ++i; continue; } // invalid leading byte

        bool valid = (i + charLen <= len);
        for (size_t j = 1; valid && j < charLen; ++j)
        {
            unsigned char cc = static_cast<unsigned char>(input[i + j]);
            if ((cc & 0xC0) != 0x80) valid = false;
        }

        if (valid)
        {
            out.append(input, i, charLen);
            i += charLen;
        }
        else
        {
            out += '?';
            ++i; // only skip the one bad byte, resync on next iteration
        }
    }

    return out;
}