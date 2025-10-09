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

	// TODO
	if (false)  {
		if (this->songs_dir.empty())  {
			return SONG_DIR_NOT_DEFINED;
		}
		
		// convert title to file name friendly format
		std::string title = song["TITLE"];
		std::replace(title.begin(), title.end(), ' ', '_');
		title = parent->getDatabase()->convert_to_ascii(title);

		// convert artist to file name friendly format
		std::string artist = song["ARTIST"];
		std::replace(artist.begin(), artist.end(), ' ', '_');
		artist = parent->getDatabase()->convert_to_ascii(artist);
		
		// TODO make it OS independent
		std::string fname = this->songs_dir + "/" + artist + "-" + title + ".txt";

		// save lyrics to file
  		std::ofstream songFile(fname);
  		songFile << this->lyrics_reg;
  		songFile.close();
	}
	else {
		song["LYRICS"] = this->lyrics_reg;

		if (parent->getDatabase()->addSong(song, true)) {
			return ADD_SONG_FAILED;
		}
	}


	return SUCCESS;
	
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
			parent->printInteractive("Successfuly added lyrics to database..."); break;
		case INVALID_ID:
			parent->printInteractive("Invalid ID to parse!", 2); break;
		case AI_ERROR_RESPONSE: 
			parent->printInteractive("Failed to gather lyrics due to ai returning error...", 2); break;
		case AI_EMPTY_RESPONSE: 
			parent->printInteractive("Failed to gather lyrics due to not returing proper response ...", 2); break;
		case ADD_SONG_FAILED: 
			parent->printInteractive("Failed to update song in database with lyrics ...", 2); break;
		case SEARCH_NO_VALID_WEBSITE: 
			parent->printInteractive("Cannot find lyrics for this song on valid websites!", 2); break;
		case PARSE_WEBSITE_FAILED:
			parent->printInteractive("Error occured when trying to parse a website"); break;
		case INVALID_GOOGLE_RESPONSE:
			parent->printInteractive("Google search contains unexpected items!"); break;
		case CURL_ERROR:
			parent->printInteractive("Internal curl error"); break;
		case OK_EXIT_CHAR: break;
		case SONG_NOT_FOUND:
			parent->printInteractive("Song with such ID not found in database"); break;
		case SEARCH_EMPTY_RESPONSE:
			parent->printInteractive("Google search returned an empty response..."); break;
		case LINK_GET_FAILED:
			parent->printInteractive("Failed to get a link from Google search json!"); break;
		case NO_ID:
			parent->printInteractive("No ID passed as an argument..."); break;
		default:
			std::cout << "Unspecified error code occured: " << error_code << std::endl;
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

	//std::cout << response << std::endl;

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
							this->lyrics_reg = formatter->parseMarkdown(raw_lyrics);
							if (!this->lyrics_reg.empty()) {
								return SUCCESS;
							}
						}

					}
				}
    		}
			return SEARCH_NO_VALID_WEBSITE;
		}
		else {
			return INVALID_GOOGLE_RESPONSE;
		}
	}
	
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

bool GatherTask::checkSanity()
{
	bool success = true;

	std::string cmds[] = {	"pandoc",
					   		"pdflatex",
					   		"curl" };

	for (int i = 0; i < 3; i++)  {
		std::string w = "which " + cmds[i] + " > /tmp/nul";
		if (system(w.c_str()))  {
			std::cout << "**Cannot use 'GatherTask', missing " << cmds[i] << " command!" << std::endl;
			success = false;
		}
	}

	// TODO check APIs and other properties for sanity
	
	return success;
}
