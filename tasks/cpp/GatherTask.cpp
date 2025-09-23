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
#include <fstream>
#include "GatherTask.h"
#include "SongBookApp.h"


// Callback to collect the response into a string
size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

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

	std::string lyrics;

	int err = searchForLyrics(lyrics, song["TITLE"], song["ARTIST"]);

	std::cout << lyrics << std::endl;
	if (err == SUCCESS)  {
		//std::cout << websites;
	}

	return SUCCESS;
	// parent->getDatabase()->addSong(song.dump(), success)
}

int GatherTask::parseByAi(std::string to_parse, std::string& from_ai)
{
	CURL* curl;
    CURLcode err_code;
    std::string read_buffer;
	nlohmann::json song;

	std::ostringstream query;
	
	query  << "I want you to take these lyrics with chords "
	       << "and parse them into a different syntax with the following rules:\n"
	       << "- Each chord line belongs to the lyric line directly below it.\n"
	       << "- Move each chord so it appears immediately before the next lyric word.\n"
	       << "- Wrap each chord in backticks (e.g., `Emi`).\n"
	       << "- Remove the original chord lines.\n"
	       << "- Verse begins with number and dot (e.g., '1. ...').\n"
	       << "- Chorus begins with '>' (e.g., '> ...').\n"
	       << "- Preserve newlines.\n"
	       << "- Output only the transformed lyrics, nothing else, inside a code block.\n\n"
	       << "```input\n"
	       << to_parse
	       << "\n```";

	nlohmann::json prompt_json = {
    	{"model", this->model},
    	{"messages", {
        	{{"role", "system"},
         		{"content", 
					"You are a strict formatter. \
						Always preserve newlines. \
						Chords in the input are written on a separate line above the lyrics. \
						For each pair (chord line + lyric line): \
						- Read the chord line left to right. \
						- Insert each chord immediately before the next word in the lyric line. \
						- Wrap the chord in backticks. (i.e. `Ami`) \
						- After inserting all chords, remove the separate chord line. \
						Rules: \
						1. Verse begins with number and dot (e.g., '1. ...'). \
						2. Chorus begins with '>' (e.g., '> ...'). \
						3. Preserve lyric words and line breaks exactly. \
						4. Output only the transformed lyrics, no explanations. \
						5. Return everything inside a single fenced code block."}},
            	{{"role", "user"},
            	{"content", query.str()}}
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

		std::cout << "Parsing by AI (" << this->model << ")" << std::endl;
        err_code = curl_easy_perform(curl);
        if(err_code != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(err_code));

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

	if (!error_code)  {
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
		std::cout << song["LYRICS"] << std::endl;

		parent->printInteractive("Do you wish to edit lyrics before saving? (y/n)", 1);
		std::string response = parent->getInput(1);

		if (response == "y")  {
			// TODO
		}
	}
	else if (error_code==1)  {
		parent->printInteractive("Invalid ID to parse!", 1);
	}
	else if (error_code==5)  {
		parent->printInteractive("Failed to gather lyrics due to ai returning error...", 2);
	}
	else if (error_code==6)  {
		parent->printInteractive("Failed to gather lyrics due to not returing proper response ...", 2);
	}
	else if (error_code==7)  {
		parent->printInteractive("Failed to update song in database with lyrics ...", 2);
	}

}

int GatherTask::searchForLyrics(std::string& lyrics, 
							 	std::string title, 
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
    		for (const auto& item : response_json["items"]) {
				for (auto& url : this->allowed_urls)  {
					if (item["displayLink"] == url)  {
						std::string link;
						try  {
							link = item["link"].get<std::string>();
						}
						catch(const std::exception& e)  {
							return LINK_GET_FAILED;
						}
						
						std::string raw_lyrics = parseWebsite(link, url);
						if (raw_lyrics == "")  {
							return PARSE_WEBSITE_FAILED;;
						}

						return parseByAi(raw_lyrics, lyrics);
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

	if (base_url == "www.velkyzpevnik.cz")  {
		// TODO
		return "";
	} else if (base_url == "pisnicky-akordy.cz")  {
		std::istringstream iss(md_content);

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