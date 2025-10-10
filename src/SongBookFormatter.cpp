#include <iostream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <format>
#include <fstream>
#include "json.hpp"
#include "SongBookFormatter.h"
#include "SongBookUtils.h"

int BardFormatter::generateSongBook(const char* output_file)
{	

	std::ostringstream toml_oss;

	toml_oss << "version = 2 \n\n"
			 << "songs = [\n";

	int i = 0;
	for (auto song : this->songs)  {
		std::string fname = std::to_string(i) + ".md";
		std::string fpath = "bard/songs/" + fname;
		// save lyrics to file

		song = SongBookUtils::getInstance()->sql2txt(song);

		std::ofstream songFile(fpath);
		songFile << song;
		songFile.close();
		i++;

		toml_oss << "    \"" << fname << "\",\n";
	}

	toml_oss << "]\n\n"
			 << "notation = \"english\"\n\n"
			 << "[[output]]\n"
			 << "file = \"songbook.pdf\"\n\n"
			 << "[book]\n"
			 << "title = \"" << SongBookUtils::getInstance()->getConfigItem("songbook/title") <<"\"\n"
			 << "subtitle = \"" << SongBookUtils::getInstance()->getConfigItem("songbook/subtitle") << "\"\n"
			 << "chorus_label = \"Ref\"\n"
			 << "title_note = \"" << SongBookUtils::getInstance()->getConfigItem("songbook/note") <<"\"";

	std::string bard_dir = SongBookUtils::getInstance()->getConfigItem("paths/bard_dir");
	
	if (bard_dir != "") {
		if (SongBookUtils::getInstance()->systemCommandExists(std::format("{}/bard", bard_dir).c_str()))  {
			std::ofstream tomlFile(std::format("{}/bard.toml", bard_dir).c_str());
			tomlFile << toml_oss.str();
			tomlFile.close();
			system(std::format("(cd {} && ./bard make )", bard_dir).c_str());
		}
		else {
			std::cout << "Command `bard` not found in bard dir" << std::endl;
		}
		
	}
	else {
		std::cout << "bard dir not found under `paths/bard_dir`" << std::endl;
	}

    return 0;
}

std::string BardFormatter::parseMarkdown(std::string markdown_lyrics)
{
	CURL* curl;
    CURLcode err_code;
    std::string read_buffer;
	nlohmann::json song;

	std::ostringstream query;

	nlohmann::json prompt_json = {
    	{"model", SongBookUtils::getInstance()->getConfigItem("ai/model")},
    	{"messages", {
        	{{"role", "system"},
         		{"content", 
					"You are a strict formatter for songbook application. Input is lyrics with chords, you parse it into different syntax.\
						Always preserve newlines. \
						Input syntax: \
						- there are chord lines and lyrics lines \
						Output syntax: \
						- Copy the lyrics lines. \
						- Ignore the chord lines.\
						Other Rules: \
						1. Verse begins with number and dot (e.g., '1. ...'). \
						2. Chorus begins with '>' (e.g., '> ...'). \
						3. Preserve lyric words and line breaks exactly. \
						5. Output only the transformed lyrics, no explanations"}},
            	{{"role", "user"},
            	{"content", markdown_lyrics}}
    		}}
	};
	std::string prompt = prompt_json.dump();
	
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth_header = "Authorization: Bearer " + SongBookUtils::getInstance()->getConfigItem("ai/api_key");
        headers = curl_slist_append(headers, auth_header.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, prompt.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, prompt.size()); 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        err_code = curl_easy_perform(curl);
        if(err_code != CURLE_OK) {
            return "";
            // return CURLE_ERROR
            // TODO print error
        }

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
			//return AI_ERROR_RESPONSE;
            // TODO print error
            return "";
		}
		else if (read_json.contains("choices")) {
		    if (!read_json["choices"].empty() && read_json["choices"][0].contains("message")) {
		        if (read_json["choices"][0]["message"].contains("content")) {
		            std::string s = read_json["choices"][0]["message"]["content"].get<std::string>();
		            if (!s.empty()) {
                        // return SUCCESS
                        // TODO print error
						return s;
		            }
					
		        }
		    } 
		}
        //return AI_EMPTY_RESPONSE;
        // TODO print error
		return "";
		
	}
	else {
        // TODO print error
		//return CURL_EMPTY_RESPONSE;
        return "";
	}
}

int BardFormatter::addSongPage(std::string title, std::string artist, std::string lyrics)
{
    std::ostringstream oss;

    oss << "# " << title << "\n"
        << "## " << artist << "\n"
        << "\n\n"
        << lyrics;

    this->songs.push_back(oss.str());

    return 0;
}

void BardFormatter::clearPages()
{   
    this->songs.clear();
    return;
}

bool BardFormatter::checkSanity()
{
    //TODO
    return false;
}

std::string LatexListFormatter::getPrintableString(std::string str)
{
  std::map<char, std::string> special_chars = 
    {{'&', "\\&"},
     {'#', "\\#"},
     {'\'', "\\'"}
    };

  std::string t;
  for (const auto& c : str)  {
    if (special_chars.count(c))  {
      t += special_chars.at(c);
    }
    else  {
      t += c;
    }
  }

  return t;
}

int LatexListFormatter::addSongPage(std::string title, std::string artist, std::string lyrics)
{
	std::ostringstream oss;    
	
	oss << getPrintableString(title)
        << " & "
		<< getPrintableString(artist) 
        << R"( \\)";

	songs.push_back(oss.str());
}

std::string LatexListFormatter::parseMarkdown(std::string markdown_lyrics)
{
	return markdown_lyrics;
}

bool LatexListFormatter::checkSanity()
{
	return false;
}

void LatexListFormatter::clearPages()
{
	this->songs.clear();
}

int LatexListFormatter::generateSongBook(const char* output_file)
{
  	std::filesystem::path tex_path = "songbook.tex";
  	std::ofstream file(tex_path);

  	file << header;
  	file << title_page;
	file << table_begin;

	int i = 0;
	for (const auto& song : this->songs)  {
		file << song;
	  
	 	if (i % SONGS_PER_PAGE == 0 && i > 0 && i < this->songs.size())  {
      		file << table_end;
      		file << table_begin;
    	}

		i++;
	}
  
	file << table_end;
  	file << ending;
	file.close();

  
  	system(std::format("pdflatex -output-directory={} {} && clear",
			".", 
            tex_path.string()
          ).c_str());
}