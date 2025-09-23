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


// Callback to collect the response into a string
size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Aux. command that calls a system command and returns its output as an std::string 
std::string execSystemCommand(const char* cmd) {
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


int GatherTask::executeCommand()
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
	
	return err;
}

int GatherTask::parseByAi(std::string to_parse, std::string& from_ai)
{
	CURL* curl;
    CURLcode err_code;
    std::string read_buffer;
	nlohmann::json song;

	std::ostringstream query;

	nlohmann::json prompt_json = {
    	{"model", this->model},
    	{"messages", {
        	{{"role", "system"},
         		{"content", 
					"You are a strict formatter for songbook application. Input is lyrics with chords, you parse it into different syntax.\
						Always preserve newlines. \
						Input syntax: \
						- there are chord lines and lyrics lines \
						- Each chord has position ABOVE some word. \
						Output syntax: \
						- each chord is put BEFORE its word \
						- each chord is wrapped in backticks. (i.e. `Ami`) \
						- There are no chord lines, chords are inserted into lyrics. \
						Other Rules: \
						1. Verse begins with number and dot (e.g., '1. ...'). \
						2. Chorus begins with '>' (e.g., '> ...'). \
						3. Preserve lyric words and line breaks exactly. \
						4. Output only the transformed lyrics, no explanations"}},
            	{{"role", "user"},
            	{"content", to_parse}}
    		}}
	};
	std::string prompt = prompt_json.dump();
	
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth_header = "Authorization: Bearer " + this->ai_api_key;
        headers = curl_slist_append(headers, auth_header.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, prompt.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, prompt.size()); 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        err_code = curl_easy_perform(curl);
        if(err_code != CURLE_OK)
            return CURL_ERROR;

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();

	nlohmann::json read_json = nlohmann::json::parse(read_buffer);

	if (!read_json.empty())  {
		if (read_json.contains("error"))  {
			if (read_json["error"].contains("message"))  {
				std::cout << "** " << read_json["error"].at("message") << " **" << std::endl;
			}
			// ai response is error
			return AI_ERROR_RESPONSE;
		}
		else if (read_json.contains("choices")) {
		    if (!read_json["choices"].empty() && read_json["choices"][0].contains("message")) {
		        if (read_json["choices"][0]["message"].contains("content")) {
		            std::string s = read_json["choices"][0]["message"]["content"].get<std::string>();
		            if (!s.empty()) {
		                from_ai = s;
						return SUCCESS;
		            }
					
		        }
		    } 
		}
		return AI_EMPTY_RESPONSE;
		
	}
	else {
		return CURL_EMPTY_RESPONSE;
	}
}

int GatherTask::startInteractive()
{
	std::string str_id;
	parent->printInteractive("Type an ID of song you want to gather lyrics for");
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
			parent->printInteractive(std::format("'{}' does not contain valid ID to parse!", str_id), 1);
			return INVALID_ID;
		}
		std::vector<std::string> s = {str_id.data()};
		updateArgument("-id", {false, s});
		
	}
	
	if (parent->getDatabase()->songExists(id))  {
		nlohmann::json song = parent->getDatabase()->getSong(id);
		parent->printInteractive(std::format("Searching lyrics for '{}' : '{}' ...", 
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

		parent->printInteractive("Found these lyrics...", 1);
		std::cout << this->lyrics_reg << std::endl;

		parent->printInteractive("Do you wish to edit lyrics before saving? (y/n)", 1);
		std::string response = parent->getInput(1);

		if (response == "y")  {

		}
	}

	switch (error_code)
	{
		case SUCCESS:
			parent->printInteractive("Successfuly added lyrics to database...");
			break;
		case INVALID_ID:
			parent->printInteractive("Invalid ID to parse!", 2);
			break;
		case AI_ERROR_RESPONSE: 
			parent->printInteractive("Failed to gather lyrics due to ai returning error...", 2);
			break;
		case AI_EMPTY_RESPONSE: 
			parent->printInteractive("Failed to gather lyrics due to not returing proper response ...", 2);
			break;
		case ADD_SONG_FAILED: 
			parent->printInteractive("Failed to update song in database with lyrics ...", 2);
			break;
		case SEARCH_NO_VALID_WEBSITE: 
			parent->printInteractive("Cannot find lyrics for this song on valid websites!", 2);
			break;
		case PARSE_WEBSITE_FAILED:
			parent->printInteractive("Error occured when trying to parse a website");
			break;
		default:
			std::cout << "Unspecifed error code occured: " << error_code << std::endl;
	}
}

int GatherTask::searchForLyrics(std::string title, 
							 	std::string artist, 
							 	int number_of_websites)
{
	
	std::ostringstream query;

	// replace spaces with '+' character, so string becomes query friendly
	std::replace(title.begin(), title.end(), ' ', '+');
	std::replace(artist.begin(), artist.end(), ' ', '+'); 

	query << "https://www.googleapis.com/customsearch/v1"
	      << "?key="
		  << this->google_api_key
		  << "&cx="
		  << this->google_search_engine
		  << "&q="
		  << parent->getDatabase()->convert_to_ascii(title) << "+"
		  << parent->getDatabase()->convert_to_ascii(artist) <<  "+"
		  << "akordy"
		  << "&num="
		  << "5";
	
	// search for valid websites
	std::string response = curlQuery(query.str().c_str());

	nlohmann::json response_json = nlohmann::json::parse(response);

	// if websites found, iterate over them to see if any of them is valid
	if (!response_json.empty())  {
		if (response_json.contains("items") && response_json["items"].is_array()) {
			for (auto& url : this->allowed_urls)  {
				for (const auto& item : response_json["items"]) {
					if (item["displayLink"] == url)  {
						std::string link;
						try  {
							link = item["link"].get<std::string>();
						}
						catch(const std::exception& e)  {
							return LINK_GET_FAILED;
						}
						
						std::string raw_lyrics = parseWebsite(link, url);
						if (raw_lyrics != "")  {
							std::cout << "Parsing by AI (" << this->model << ")" << std::endl;
							// TODO make this a thread
							return parseByAi(raw_lyrics, this->lyrics_reg);
						}

					}
				}
    		}
			return SEARCH_NO_VALID_WEBSITE;
		}
	}
	else {
		return CURL_EMPTY_RESPONSE;
	}
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
	std::string md_content = execSystemCommand(cmd.c_str());

	std::istringstream iss(md_content);
	std::cout << "Parsing website " << website_url << std::endl;
	if (base_url == "www.velkyzpevnik.cz")  {

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
			if (line == "``` {#chordbox .format style=\"width: 59ch\"}")  {
				gather_lyrics = true;
			} 
		}
		return _lyrics.str();
	} else if (base_url == "pisnicky-akordy.cz")  {

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