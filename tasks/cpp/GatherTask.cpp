#include <string>
#include <iostream>
#include <format>
#include "json.hpp"
#include <iomanip>
#include <curl/curl.h>
#include <filesystem>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <cstdarg>
#include <thread>
#include <fstream>
#include "GatherTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"


// Callback to collect the response into a string
size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int GatherTask::Execute(char command)
{
      switch(command)  {
        case 'g': gatherSong();
				  break;
        default: return 1;
      }

      return 0;
}

void GatherTask::gatherSong()
{
	CURL* curl;
    CURLcode err_code;
    std::string read_buffer;
	

	windows["Log Screen"]->Clear();
	int id = stoi(SongBookUtils::getInstance()->getConfigItem("workspace/current_id", "-1"));

	if (parent->getDatabase()->songExists(id))  {
		nlohmann::json song;

		song = parent->getDatabase()->getSong(id);
		
		windows["Log Screen"]->Print(std::format("Searching lyrics for song ({}, {})...", song["TITLE"].get<std::string>(), song["ARTIST"].get<std::string>()));
		int err = searchForLyrics(song["TITLE"], song["ARTIST"]);
		
		if (!err)  {
			windows["Log Screen"]->Print("Found these lyrics... Do you want to keep them? (y/n)");
			windows["Main Screen"]->Clear();
			windows["Main Screen"]->Print(this->lyrics_reg);
			std::string choice = windows["Log Screen"]->GetString();
			
			if (choice == "y")  {
				song["LYRICS"] = this->lyrics_reg;
				if (!parent->getDatabase()->addSong(song, true)) {
					windows["Log Screen"]->Print("Song successfully gathered...");
					return;
				}
				else {
					windows["Log Screen"]->Print("Unexpected error when adding song back to database...");
				}
			} else {
				return;
			}
		}
		else {
			windows["Log Screen"]->Print("Error when gathering lyrics...");
			return;
		}
		






	}

	/*
	TODO contemplate this argument
	if (SongBookUtils::getInstance()->getConfigItem("paths/songs_dir") != "")  {
		
		// convert title to file name friendly format
		std::string title = song["TITLE"];
		std::replace(title.begin(), title.end(), ' ', '_');
		title = parent->getDatabase()->convert_to_ascii(title);

		// convert artist to file name friendly format
		std::string artist = song["ARTIST"];
		std::replace(artist.begin(), artist.end(), ' ', '_');
		artist = parent->getDatabase()->convert_to_ascii(artist);
		
		// TODO make it OS independent
		std::string fname = SongBookUtils::getInstance()->getConfigItem("paths/songs_dir") + "/" + artist + "-" + title + ".md";

		// save lyrics to file
  		std::ofstream songFile(fname);
  		songFile << this->lyrics_reg;
  		songFile.close();
	}
	*/
}

int GatherTask::searchForLyrics(std::string title, 
							 	std::string artist, 
							 	int number_of_websites)
{
	
	// replace spaces with '+' character, so string becomes query friendly
	std::replace(title.begin(), title.end(), ' ', '+');
	std::replace(artist.begin(), artist.end(), ' ', '+'); 
	

	for (auto& website : this->allowed_websites)  {
	  std::string lowered_website = website;
	  std::replace(lowered_website.begin(), lowered_website.end(), ' ', '+');   
	  std::ostringstream query;
	  query << "https://serpapi.com/search?engine=google"
	  	  << "&api_key="
	  	  << SongBookUtils::getInstance()->getConfigItem("google/api_key")
	  	  << "&q="
	  	  << SongBookUtils::getInstance()->convert_to_ascii(title) << "+"
	  	  << SongBookUtils::getInstance()->convert_to_ascii(artist) <<  "+"
	  	  << SongBookUtils::getInstance()->convert_to_ascii(lowered_website);
	  
	  // search for valid websites
	  std::string response = curlQuery(query.str().c_str());
  
	  nlohmann::json response_json = nlohmann::json::parse(response);
  
	  if (response_json.empty())  {
	    SongBookUtils::getInstance()->printError("GatherTask: The JSON query response is empty object!");
	    return INVALID_GOOGLE_RESPONSE;
	  }
  
	  if (response_json.contains("organic_results") && response_json["organic_results"].is_array()) {
	  	for (const auto& item : response_json["organic_results"]) {
		  if (item["source"] == website)  {
		    std::string link;
		    try  {
		  	  link = item["link"].get<std::string>();
		    }
		    catch(const std::exception& e)  {
		  	  SongBookUtils::getInstance()->printError("GatherTask: Could not obtain valid value for key 'link', not a std::string");
		  	  return LINK_GET_FAILED;
		    }
		    
		    std::string raw_lyrics = parseWebsite(link, website);
		    if (raw_lyrics != "")  {
		  	  windows["Log Screen"]->Print(std::format("Parsing by AI ({})", SongBookUtils::getInstance()->getConfigItem("ai/model")));
		  	  // TODO make this a thread
		  	  this->lyrics_reg = formatter->parseMarkdown(raw_lyrics);
		  	  if (!this->lyrics_reg.empty()) {
		  	    return SUCCESS;
		  	  }
			  else {
				return INVALID_WEBSITE_PARSE;
			  }
		    }
		  }
	  	}
	  	return SEARCH_NO_VALID_WEBSITE;
	  	  }
	  else {
	    SongBookUtils::getInstance()->printError(std::format("GatherTask: Invalid google response: '{}', missing 'items' key", response));
	    return INVALID_GOOGLE_RESPONSE;
	  }

	}

	SongBookUtils::getInstance()->printError("GatherTask: CURL operation returned and empty response");
	return CURL_EMPTY_RESPONSE;
}

std::string GatherTask::curlQuery(const char* query)
{
	CURL* curl;
    CURLcode err_code;
	std::string response;

	curl = curl_easy_init();

	if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, query);

        // Set callback function to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform request
        err_code = curl_easy_perform(curl);

        // Check for errors
        if(err_code != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " 
                      << curl_easy_strerror(err_code) << std::endl;
			return "";
        }
    }
	else {
		return "";
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return response;
}



std::string GatherTask::parseWebsite(std::string website_url, std::string base_url)
{

	std::string html_content = curlQuery(website_url.c_str());
	// This is just wrong, but none of other options I found did not work better than this.
    // Pandoc is best tool for parsing html to markdown and it cannot be used as a lib.	
	std::string cmd = "curl -s " + website_url + " | pandoc -f html -t markdown";
	std::string md_content = SongBookUtils::getInstance()->execSystemCommand(cmd.c_str());

	std::istringstream iss(md_content);
	windows["Log Screen"]->Print(std::format("Parsing website '{}'", website_url));
	if (base_url == "Velký zpěvník")  {

		bool gather_lyrics = false;
		std::ostringstream _lyrics;

		for (std::string line; std::getline(iss, line); ) {
			if (gather_lyrics)  {
				if (line == "```")  {
					break;
				} else {
					_lyrics << line << '\n';
				}
			}

			if (line.substr(0, 14) == "``` {#chordbox")  {
				gather_lyrics = true;
			} 
		}
		return _lyrics.str();
	} else if (base_url == "Písničky s akordy")  {

		bool gather_lyrics = false;
		std::ostringstream _lyrics;

		for (std::string line; std::getline(iss, line); ) {
			if (gather_lyrics)  {
				if (line == ":::")  {
					break;
				} else {
					_lyrics << line << '\n';
				}
			}
			if (line == "::: {#songtext}")  {
				gather_lyrics = true;
			} 
		}

		return _lyrics.str();
	} else {
		return "";
	}
}

bool GatherTask::checkSanity()
{
	bool success = true;

	std::string cmds[] = {	"pandoc",
					   		"pdflatex",
					   		"curl" };

	for (int i = 0; i < 3; i++)  {
		if (!SongBookUtils::getInstance()->systemCommandExists(cmds[i].c_str()))  {
			windows["Log Screen"]->Print(std::format("**Cannot use 'GatherTask', missing '{}' command", cmds[i]));
			success = false;
		}
	}

	// TODO check APIs and other properties for sanity
	return success;
}
