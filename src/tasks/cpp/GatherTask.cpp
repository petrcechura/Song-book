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
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <thread>
#include <utility>
#include <form.h>
#include <fstream>
#include "GatherTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "SongEditorServer.hpp"


// Callback to collect the response into a string
size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int GatherTask::Execute(char command)
{	
	switch(parent->getState())  {
		case SongBookApp::app_state_t::SONG_BROWSE:
			switch(command)  {
				case 'g': gatherSong();
				break;
			}
		parent->addTaskLegend('g', "Gather lyrics for a song");
		break;
		case SongBookApp::app_state_t::COLLECTION_BROWSE:
			break;
	}
	return 0;
}

void GatherTask::gatherSong()
{
	CURL* curl;
    CURLcode err_code;
    std::string read_buffer;
	

	windows["Log Screen"]->Clear();
	int id = stoi(SongBookUtils::getInstance()->getConfigItem("workspace/current_song_id", "-1"));

	if (parent->getDatabase()->songExists(id))  {
		nlohmann::json song;

		song = parent->getDatabase()->getSong(id);
		
		windows["Log Screen"]->Print(std::format("Searching lyrics for song ({}, {})...", song["TITLE"].get<std::string>(), song["ARTIST"].get<std::string>()));
		int err = searchForLyrics(song["TITLE"], song["ARTIST"]);

		if (!err)  {
			windows["Log Screen"]->Print("Found these lyrics...");
			this->lyrics_reg = parent->SongEditor(SongBookUtils::sql2txt(this->lyrics_reg));
			refresh();
			
			windows["Log Screen"]->Clear();
			windows["Log Screen"]->Print("Do you want to keep those lyrics? (y/n)");
			std::string choice = windows["Log Screen"]->GetString(1);
			
			if (choice == "y")  {
				song["LYRICS"] = this->lyrics_reg;
				if (!parent->getDatabase()->addSong(song, true)) {
					windows["Log Screen"]->Print("Song successfully gathered...");
					
					nlohmann::json songs = parent->getDatabase()->getJson();
  					SongBookUtils::setConfigJson("workspace/songs", songs);
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
		  	  //this->lyrics_reg = formatter->parseMarkdown(raw_lyrics);
			  this->lyrics_reg = raw_lyrics;
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
    windows["Log Screen"]->Print(std::format("Parsing website '{}'", website_url));

    std::string html_content = curlQuery(website_url.c_str());
    if (html_content.empty()) {
        return "";
    }
    // parse HTML from memory
    htmlDocPtr doc = htmlReadMemory(
        html_content.c_str(),
        html_content.size(),
        nullptr,
        nullptr,
        HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING
    );

    if (!doc) {
        return nullptr;
    }

    xmlXPathContextPtr context = xmlXPathNewContext(doc);

	std::string lyrics;
	
	if (base_url==allowed_websites[0])  {
		lyrics = parseHtmlWebsiteNo0(context);
	}

    // cleanup
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    xmlCleanupParser();

	return lyrics;
}

// ================================
// Písničky s akordy (pisnicky-akordy.cz)
// ================================
std::string GatherTask::parseHtmlWebsiteNo0(xmlXPathContextPtr context)
{	
    xmlXPathObjectPtr result = xmlXPathEvalExpression(
        (const xmlChar*)"//div[@id='songtext']//pre",
        context
    );

	// TODO
	std::ostringstream lyrics;

    if (result && result->nodesetval && result->nodesetval->nodeNr > 0) {
        xmlNode* pre = result->nodesetval->nodeTab[0];
		
		std::string chords = "";
   		for (xmlNode* cur = pre->children; cur; cur = cur->next) {

			// ELEMENT node
       		if (cur->type == XML_ELEMENT_NODE) {
				std::ostringstream chord_line;
				for (xmlNode *inner = cur->children; inner; inner = inner->next)  {
					xmlChar* chord = xmlNodeGetContent(inner);
					chord_line << (const char*) chord; 
				}

				if (!chords.empty())  {
					lyrics << formatter->processSingleChordLine(chords);
				}
				chords = chord_line.str();
			}

			// TEXT node
       		else if (cur->type == XML_TEXT_NODE) {

				if (!chords.empty())  {
					lyrics << formatter->processTextWithChords(chords, (const char*)cur->content);
				} else {
					lyrics << formatter->processTextLine((const char*)cur->content);
				}
				chords.clear();
       		}
		}

    if (result) 
		xmlXPathFreeObject(result);
	}

	return lyrics.str();
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
