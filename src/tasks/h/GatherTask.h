#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <utility>
#include <functional>
#include <map>
#include "SongBookFormatter.h"
#include "WindowServer.h"
#include "Window.h"
#include "WTask.h"
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

class SongBookApp;


class GatherTask : public WTask<SongBookApp>
{
public:
  GatherTask(std::string name, SongBookApp* parent, std::string description) 
    : WTask<SongBookApp>(name, parent, description) { this->formatter = new BardFormatter(); };

    // virtual functions override
    virtual int Execute(char command) override;

    void gatherSong();

    std::string curlQuery(const char* query);

    /** Searches Google with songs properties, returns websites that contain song chords */
    int searchForLyrics(std::string title, 
                        std::string artist,
                        int number_of_websites=5);
    
    /** Parses a websites using pandoc tool, returning chords only in plain text */
    std::string parseWebsite(std::string website_url, std::string base_url);

    /** Check whether this OS has all dependencies for all `Gather Task` functions
        TODO: Not in use right now! Implement and use... */
    bool checkSanity();

    int deleteThis();

  private:

    enum {
      SUCCESS,
      OK_EXIT_CHAR,
      CURL_ERROR,
      SONG_NOT_FOUND,
      INVALID_ID,
      NO_ID,
      ADD_SONG_FAILED,
      AI_ERROR_RESPONSE,
      AI_NO_RESPONSE,
      AI_EMPTY_RESPONSE,
      CURL_EMPTY_RESPONSE,
      SEARCH_EMPTY_RESPONSE,
      SEARCH_NO_VALID_WEBSITE,
      LINK_GET_FAILED,
      PARSE_WEBSITE_FAILED,
      INVALID_GOOGLE_RESPONSE,
      LYRICS_NOT_SAVED,
      SONG_DIR_NOT_DEFINED,
      INVALID_WEBSITE_PARSE
    } ErrorCode;

    // TODO 
    SongBookFormatter* formatter;

    // Google search related properties
    std::string google_api_key = "";
    std::string google_search_engine = "";
    std::string allowed_websites[2] = {
      "Písničky s akordy",
      "Velký zpěvník"
    };

    // Ai parsing related properties
    std::string temp_fp = "";
    std::string lyrics_reg;

    std::vector<std::pair<std::string, std::string>> parseHtmlWebsiteNo0(xmlXPathContextPtr context);
};