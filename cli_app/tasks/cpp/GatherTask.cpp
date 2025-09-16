#include <string>
#include <iostream>
#include <format>
#include "json.hpp"
#include <iomanip>
#include <curl/curl.h>
#include <sstream>
#include "GatherTask.h"
#include "SongBookApp.h"

// Callback to collect the response into a string
size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
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
			return 1;
		}

		if (parent->getDatabase()->songExists(id))  {
			song = parent->getDatabase()->getSong(id);
			this->setSongProps(song["TITLE"], song["ARTIST"]);
		}
		else  {
			// song with -id does not exist
			return 3;
		}
		
	}
	else  {
		// -id not set
		return 2;
	}


	// TODO move this whole code block into separate method 
	std::ostringstream oss;
	oss << "{\"model\": \"" << this->model << "\"," 
		<<  "\"messages\": ["
		<< "{\"role\" : \"user\","
		<< " \"content\" : \"" << this->ai_prompt << "\"}"
		<< "]}";
    std::string prompt = oss.str();

	
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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        err_code = curl_easy_perform(curl);
        if(err_code != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(err_code));

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();

	nlohmann::json read_json = nlohmann::json::parse(read_buffer);

	std::cout << read_buffer << std::endl;

	bool success = false;
	if (!read_json.empty())  {
		if (read_json.contains("error"))  {
			if (read_json["error"].contains("message"))  {
				std::cout << "** " << read_json["error"].at("message") << " **" << std::endl;
			}
			// ai response is error
			return 5;
		}
		else if (read_json.contains("choices")) {
		    if (!read_json["choices"].empty() && read_json["choices"][0].contains("message")) {
		        if (read_json["choices"][0]["message"].contains("content")) {
		            std::string s = read_json["choices"][0]["message"]["content"].get<std::string>();
		            if (!s.empty()) {
		                song["LYRICS"] = s;
		                success = true;
		            }
		        }
		    }
		}

		if (success)  {
			// success
			if(!parent->getDatabase()->addSong(song.dump(), success))  {
				return 0;
			}  
			else {
				// failed to add updated version to database
				return 7;
			}
			
			return 0;
		}
		else {
			// failed to get response from ai
			return 6;
		}
	}

    return 0;
}


// TODO replace this function with an AI query request. In a request, there will be a string 
// from parsed html response from curl, so AI will just transform text and chords into markdown syntax
void GatherTask::setSongProps(std::string title, std::string artist)
{
	std::ostringstream oss;
	/*
	oss << "I want you to search for lyrics for song with these properties:"
		<< " Title: '" << title << "', Artist: '" << artist 
		<< "'\\n You can try multiple urls, just make sure lyrics are right."
        << "\\nNow, extract lyrics, song title and song artist and convert it"
        << "to following format. Ignore the chords. See that every verse begin with (number)"
        << "and chorus with >" 
        << "\\n\\nSYNTAX EXAMPLE BEGIN"
        << "\\n\\n# (Song title)" 
        << "\\n## (Song artist)"
        << "\\n\\n1. O the summer time has come" 
        << "\\nAnd the trees are sweetly bloomin"
        << "\\nAnd the wild mountain thyme" 
        << "\\nGrows around the bloomin heather"
        << "\\nWill ye go lassie go?" 
        << "\\n\\n> And we'll all go together to pull wild mountain thyme" 
        << "\\nAll around the bloomin heather, will ye go lassie go?"
        << "\\n\\n2. I will build my love a bower" 
        << "\\nBy yon cool crystal fountain"
        << "\\nAnd round it I will pile"
        << "\\nAll the wild flowers o the mountain." 
        << "\\nWill ye go lassie go?" 
        << "\\n\\nSYNTAX EXAMPLE END" 
        << "\\nReturn ONLY result, no other talking.";
	*/
	oss << "I want you to access this URL: (https://pisnicky-akordy.cz/nedvedi/frankie-dlouhan)"
		<< "If there is a problem with access, try again, it should work."
        << "\\nNow, extract lyrics, song title and song artist and convert it"
        << "to following format. Ignore the chords. See that every verse begin with (number)"
        << "and chorus with >" 
        << "\\n\\nSYNTAX EXAMPLE BEGIN"
        << "\\n\\n# (Song title)" 
        << "\\n## (Song artist)"
        << "\\n\\n1. O the summer time has come" 
        << "\\nAnd the trees are sweetly bloomin"
        << "\\nAnd the wild mountain thyme" 
        << "\\nGrows around the bloomin heather"
        << "\\nWill ye go lassie go?" 
        << "\\n\\n> And we'll all go together to pull wild mountain thyme" 
        << "\\nAll around the bloomin heather, will ye go lassie go?"
        << "\\n\\n2. I will build my love a bower" 
        << "\\nBy yon cool crystal fountain"
        << "\\nAnd round it I will pile"
        << "\\nAll the wild flowers o the mountain." 
        << "\\nWill ye go lassie go?" 
        << "\\n\\nSYNTAX EXAMPLE END" 
        << "\\nReturn ONLY result, no other talking.";

	this->ai_prompt = oss.str();
}

int GatherTask::startInteractive()
{
	std::string str_id;
	parent->printInteractive("Type an ID of song you want to gather lyrics for");
	str_id = parent->getInput(1);

	int id;

  	if (str_id == std::string(1,parent->getExitChar()))  {
    	return 0;
  	}
	else  {
		try {
			id = std::stoi(str_id);
		}
		catch (const std::invalid_argument& e)  {
			parent->printInteractive(std::format("'{}' does not contain valid ID to parse!", str_id), 1);
			return 1;
		}
		std::vector<std::string> s = {str_id.data()};
		updateArgument("-id", {false, s});
		
	}
	
	if (parent->getDatabase()->songExists(id))  {
		nlohmann::json song = parent->getDatabase()->getSong(id);
		parent->printInteractive(std::format("Searching lyrics for '{}' : '{}' ...", 
												song["TITLE"].get<std::string>(), 
												song["ARTIST"].get<std::string>()));
		return 0;

	}
	else  {
		return 1;
	}
	
	return 1;
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

std::string GatherTask::googleSearch(std::string title, std::string artist, int number_of_websites)
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
		  << title << "+"
		  << artist <<  "+"
		  << "akordy"
		  << "&num="
		  << "5";

	CURL* curl;
    CURLcode err_code;
    std::string response;
	
    curl = curl_easy_init();

	if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, query.str());

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
        } else {
            std::cout << "Response:\n" << response << std::endl;
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

	// TODO work with 'response', check if there is an allowed websites in returned websites...
	// -> check if website in allowed_websites
	// then call `parseWebsite` if so

	
}

