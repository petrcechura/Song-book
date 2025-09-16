#include <string>
#include <filesystem>
#include <iostream>
#include "cmdapputils.h"

class SongBookApp;


/** Gather lyrics for particular song using AI tool */
class GatherTask : public Task<SongBookApp>
{
  public:

    GatherTask(std::string cmd, SongBookApp* parent) 
      : Task<SongBookApp>(cmd, parent) {};
 
    int startInteractive() override;
    int executeCommand() override;
    void endInteractive(int error_code) override;

    void setAiApiKey(std::string ai_api_key) { this->ai_api_key = ai_api_key; };
    void setGoogleApiKey(std::string google_api_key) { this->google_api_key = google_api_key; };
    void setGoogleSearchEngine(std::string google_search_engine) { this->google_search_engine = google_search_engine; };
    void setSongProps(std::string title, std::string artist);
    void setAiModel(std::string model) { this->model = model; };

    /** Searches Google with songs properties, returns websites that contain song chords */
    std::string googleSearch(std::string title, std::string artist, int number_of_websites);
    
    /** Parses a websites using pandoc tool, returning chords only in plain text */
    std::string parseWebsite(std::string website_url);



  private:


    // Google search related properties
    std::string google_api_key = "";
    std::string google_search_engine = "";
    std::string allowed_urls[2] = {
      "https://www.velkyzpevnik.cz",
      "https://pisnicky-akordy.cz"
    };

    // Ai parsing related properties
    std::string model = "gpt-4.1-mini";
    std::string ai_api_key = "";
    std::string temp_fp = "";
    std::string ai_prompt = "\
        I want you to read this URL ({}}), containing a song lyrics. If the url is unaccessable, try again, \
        it should work. Now, extract lyrics, song title and song artist and convert it \
        to following format. Ignore the chords. See that every verse begin with (number). \
        and chorus with > \
        \
        SYNTAX EXAMPLE BEGIN  \
        # (Song title)  \
        ## (Song artist)  \
        \
        1. O the summer time has come \
        And the trees are sweetly bloomin  \
        And the wild mountain thyme \
        Grows around the bloomin heather \
        Will ye go lassie go? \
        \
        > And we'll all go together to pull wild mountain thyme \
        All around the bloomin heather, will ye go lassie go? \
        \
        2. I will build my love a bower \
        By yon cool crystal fountain \
        And round it I will pile \
        All the wild flowers o the mountain. \
        Will ye go lassie go? \
        SYNTAX EXAMPLE END \
        \
        Return ONLY result, no other talking.";
};
