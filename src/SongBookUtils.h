#include <string>
#include <iostream>
#include "SongDatabase.h"
#include "SongBookFormatter.h"
#include "json.hpp"

#ifndef SONGBOOKUTILS_H
#define SONGBOOKUTILS_H

#define TITLE_WIDTH 30
#define ARTIST_WIDTH 30

class SongBookUtils
{
  public:
    /** A `singleton` method to get unique shared instance in codebase */
    static SongBookUtils* getInstance();


    // =============================
    // ===== CONFIG FILE UTILS =====
    // =============================

    static std::string getConfigItem(std::string path, std::string def = "");
    static void setConfigItem(std::string path, std::string value);
    static int setJson(nlohmann::json _json) { config = _json; return 0; };
    static nlohmann::json getJson() {return config; };

    // =============================
    // ======== System calls =======
    // =============================

    // Aux. command that calls a system command and returns its output as an std::string 
    static std::string execSystemCommand(const char* cmd);
    /** Checks if a system command exists on this machine */
    static bool systemCommandExists(const char* cmd);

    // ==============================
    // ====== TEXT PROCESSING =======
    // ==============================

    /** Converts a piece of text from format that is stored inside sql (preserving newlines and other
        special character) into readable syntax */
    static std::string sql2txt(std::string sql);
    /** Prepares a string for saving into sql database */
    static std::string txt2sql(std::string txt);

    static std::string convert_to_ascii(std::string str);

    /** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
    static inline int countStringChars(const std::string& _str);

    /** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
    static inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30);

    
    // ==============================
    // ====== PRINTING METHODS ======
    // ==============================

    static std::string printSong(const std::string& no, const std::string& name, const std::string& author, bool has_lyrics);
    static std::string printSongListHeader();
    static std::string printSongListBottom();   
    static void printInteractive(const std::string& text, unsigned int indentation=0, bool newline=true);
    /** Prints the error message into std::cerr */
    static void printError(std::string msg);

    /** Creates a scrollable window in terminal, using arrows for  */
    static void scrollInterractive(const std::vector<std::string> lines, const std::string& header, int lines_count);

  private:
    static nlohmann::json config;
    static SongBookUtils* _utils;
    SongBookUtils() {};
    static const char config_delimiter;

    static nlohmann::json* parseConfigPath(nlohmann::json* j, std::string path);
};


#endif
