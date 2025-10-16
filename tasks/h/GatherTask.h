#include <string>
#include <filesystem>
#include <iostream>
#include "cmdapputils.h"
#include "SongBookFormatter.h"

class SongBookApp;


/** Gather lyrics for particular song using AI tool */
class GatherTask : public Task<SongBookApp>
{
  public:

    GatherTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {
        this->formatter = new BardFormatter();
      };

    // virtual functions override
    int startInteractive() override;
    int executeCommand(int error_code) override;
    void endInteractive(int error_code) override;

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
      SONG_DIR_NOT_DEFINED
    } ErrorCode;

    // TODO 
    SongBookFormatter* formatter;

    // Google search related properties
    std::string google_api_key = "";
    std::string google_search_engine = "";
    std::string allowed_urls[2] = {
      "pisnicky-akordy.cz",
      "www.velkyzpevnik.cz"
    };

    // Ai parsing related properties
    std::string temp_fp = "";
    std::string lyrics_reg;
};
