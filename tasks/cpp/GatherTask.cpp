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


int GatherTask::executeCommand(int error_code)
{	
	CURL* curl;
    CURLcode err_code;
    std::string read_buffer;
	nlohmann::json song;

	if (argumentExists("-id", true))  {
		arg_store_t a = getArgument("-id");

		int id;
		try
		{
			id = std::stoi(a.values[0]);
		}
		catch(const std::exception& e)
		{
			// -id is invalid to parse
			return INVALID_ID;
		}

		if (parent->getDatabase()->songExists(id))  {
			song = parent->getDatabase()->getSong(id);
		}
		else  {
			// song with -id does not exist
			return SONG_NOT_FOUND;
		}
		
	}
	else  {
		// -id not set
		return NO_ID;
	}

	int err = searchForLyrics(song["TITLE"], song["ARTIST"]);
	
	if (err != SUCCESS) {
		return err;
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
	
	song["LYRICS"] = this->lyrics_reg;
	if (parent->getDatabase()->addSong(song, true)) {
		return ADD_SONG_FAILED;
	}

	return SUCCESS;
}

int GatherTask::startInteractive()
{
	std::string str_id;
	SongBookUtils::getInstance()->printInteractive("Type an ID of song you want to gather lyrics for");
	str_id = parent->getInput(1);

	int id;

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return OK_EXIT_CHAR;
  	}
	else  {
		try {
			id = std::stoi(str_id);
		}
		catch (const std::invalid_argument& e)  {
			SongBookUtils::getInstance()->printInteractive(std::format("'{}' does not contain valid ID to parse!", str_id), 1);
			return INVALID_ID;
		}
		std::vector<std::string> s = {str_id.data()};
		updateArgument("-id", {false, s});
		
	}
	
	if (parent->getDatabase()->songExists(id))  {
		nlohmann::json song = parent->getDatabase()->getSong(id);
		SongBookUtils::getInstance()->printInteractive(std::format("Searching lyrics for '{}' : '{}' ...", 
												song["TITLE"].get<std::string>(), 
												song["ARTIST"].get<std::string>()));
		return SUCCESS;

	}
	else  {
		return SONG_NOT_FOUND;
	}
}

void GatherTask::endInteractive(int error_code)
{

	if (error_code == SUCCESS)  {
		arg_store_t a = getArgument("-id");

		int id;
		try
		{
			id = std::stoi(a.values[0]);
		}
		catch(const std::exception& e)
		{
			return;
		}
		nlohmann::json song = parent->getDatabase()->getSong(id);

		SongBookUtils::getInstance()->printInteractive("Found these lyrics:", 1);
		std::cout << this->lyrics_reg << std::endl;
		SongBookUtils::getInstance()->printInteractive("Do you want to keep them? (y/n)", 1);
		std::string i = parent->getInput();
		if (i != "y")  {
			song.erase("LYRICS");
			if (parent->getDatabase()->addSong(song, true))  {
				error_code = ADD_SONG_FAILED;
			}
			else {
				error_code = LYRICS_NOT_SAVED;
			}
		}
	}

	switch (error_code)
	{
		case SUCCESS:
			SongBookUtils::getInstance()->printInteractive("Successfuly added lyrics to database..."); break;
		case INVALID_ID:
			SongBookUtils::getInstance()->printInteractive("Invalid ID to parse!", 2); break;
		case AI_ERROR_RESPONSE: 
			SongBookUtils::getInstance()->printInteractive("Failed to gather lyrics due to ai returning error...", 2); break;
		case AI_EMPTY_RESPONSE: 
			SongBookUtils::getInstance()->printInteractive("Failed to gather lyrics due to not returing proper response ...", 2); break;
		case ADD_SONG_FAILED: 
			SongBookUtils::getInstance()->printInteractive("Failed to update song in database with lyrics ...", 2); break;
		case LYRICS_NOT_SAVED:
			SongBookUtils::getInstance()->printInteractive("Lyrics not saved...", 2); break; 
		case SEARCH_NO_VALID_WEBSITE: 
			SongBookUtils::getInstance()->printInteractive("Cannot find lyrics for this song on valid websites!", 2); break;
		case PARSE_WEBSITE_FAILED:
			SongBookUtils::getInstance()->printInteractive("Error occured when trying to parse a website"); break;
		case INVALID_GOOGLE_RESPONSE:
			SongBookUtils::getInstance()->printInteractive("Google search contains unexpected items!"); break;
		case CURL_ERROR:
			SongBookUtils::getInstance()->printInteractive("Internal curl error"); break;
		case OK_EXIT_CHAR: break;
		case SONG_NOT_FOUND:
			SongBookUtils::getInstance()->printInteractive("Song with such ID not found in database"); break;
		case SEARCH_EMPTY_RESPONSE:
			SongBookUtils::getInstance()->printInteractive("Google search returned an empty response..."); break;
		case LINK_GET_FAILED:
			SongBookUtils::getInstance()->printInteractive("Failed to get a link from Google search json!"); break;
		case NO_ID:
			SongBookUtils::getInstance()->printInteractive("No ID passed as an argument..."); break;
		case INVALID_WEBSITE_PARSE:
			SongBookUtils::getInstance()->printInteractive("Could not parse lyrics from a website..."); break;
		default:
			SongBookUtils::getInstance()->printError(std::format("Unspecified error code occured: {}", error_code));
	}
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
		  	  std::cout << "Parsing by AI (" << SongBookUtils::getInstance()->getConfigItem("ai/model") << ")" << std::endl;
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
	std::string cmd = "curl " + website_url + " | pandoc -f html -t markdown";
	std::string md_content = SongBookUtils::getInstance()->execSystemCommand(cmd.c_str());

	std::istringstream iss(md_content);
	std::cout << "Parsing website " << website_url << std::endl;
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
			std::cout << "**Cannot use 'GatherTask', missing " << cmds[i] << " command!" << std::endl;
			success = false;
		}
	}

	// TODO check APIs and other properties for sanity
	return success;
}
