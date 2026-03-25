#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>
#include <curl/curl.h>
#include <format>
#include <filesystem>
#include <fstream>
#include <map>
#include "json.hpp"
#include "SongBookFormatter.h"
#include "SongBookUtils.h"

int BardFormatter::exportSongs(const char* output_dir)
{	

	// create output directory
	if (!std::filesystem::exists(output_dir))  {
		std::filesystem::create_directory(output_dir);
	}

	// create directory for songs
	std::string songs_dir = std::format("{}/songs", output_dir);
	if (!std::filesystem::exists(songs_dir))  {
		std::filesystem::create_directory(songs_dir);
	}

	std::ostringstream toml_oss;

	toml_oss << "version = 2 \n\n"
			 << "songs = [\n";

	// create a markdown file for each song
	int i = 0;
	for (auto song : this->songs)  {

		// convert title to file name friendly format
		std::string title = song["TITLE"];
		std::replace(title.begin(), title.end(), ' ', '-');
		title = SongBookUtils::getInstance()->convert_to_ascii(title);

		// convert artist to file name friendly format
		std::string artist = song["ARTIST"];
		std::replace(artist.begin(), artist.end(), ' ', '-');
		artist = SongBookUtils::getInstance()->convert_to_ascii(artist);

		// prepare lyrics
		std::ostringstream lyrics;
    	lyrics << "# " << SongBookUtils::getInstance()->sql2txt(song["TITLE"]) << "\n"
        	<< "## " << SongBookUtils::getInstance()->sql2txt(song["ARTIST"]) << "\n"
        	<< "\n\n"
        	<< SongBookUtils::getInstance()->sql2txt(song["LYRICS"]);

		std::string fname = title + "_" + artist + ".md";
		std::string fpath = songs_dir + "/" + fname;
		
		// save lyrics to file
		std::ofstream songFile(fpath);
		songFile << lyrics.str();
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
	
	
	// create TOML file
	std::ofstream tomlFile(std::format("{}/bard.toml", output_dir).c_str());
	tomlFile << toml_oss.str();	
	tomlFile.close();

    std::string bard_tool = SongBookUtils::getConfigItem("paths/bard_command", "");
    if (bard_tool.empty())  {
      return 1;
    }

    std::string s = SongBookUtils::execSystemCommand(std::format("(cd {} && {} init)", output_dir, bard_tool).c_str());
	SongBookUtils::printError(s);

    return 0;

}

int BardFormatter::generateSongBook(const char* output_dir)
{	
  std::string bard_tool = SongBookUtils::getConfigItem("paths/bard_command", "");

  if (bard_tool.empty())  {
    return 1;
  }

  if (!std::filesystem::exists(output_dir))  {
  	return 2;
  }	
  
 
  SongBookUtils::execSystemCommand(std::format("(cd {} && {} make)", output_dir, bard_tool).c_str());
  SongBookUtils::printError(std::format("(cd {} && {} make)", output_dir, bard_tool).c_str());

  // TODO create symbolink link to work?

  return 0;
}

// TODO remove
std::string BardFormatter::processChordLines(std::string lyrics)
{
    bool is_chord_line = false;

    std::map<int, std::string> chord_marks;

    std::string chord = "";
    int mark = 0;
    std::string lines = "";

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string width_lyrics = conv.from_bytes(lyrics);

    int c_num = 0;
    for (char32_t c : width_lyrics)
    {
        std::string utf8_char = conv.to_bytes(c);

        if (c == U'\n') {
            if (!is_chord_line) {
                lines += "\n";
                chord_marks.clear();
            }
            c_num = 0;
            is_chord_line = false;
        }
        else if (c == U'$') {
            is_chord_line = true;
        }
        else {
            if (is_chord_line) {
                if (c == U' ') {
                    if (!chord.empty()) {
                        chord_marks[mark] = chord;
                        chord.clear();
                    }
                }
                else {
                    if (chord.empty())
                        mark = c_num;
                    chord += utf8_char;
                }
            }
            else {
                if (chord_marks.count(c_num)) {
                    lines += "`" + chord_marks[c_num] + "`" + utf8_char;
                }
                else {
                    lines += utf8_char;
                }
            }
        }
        c_num++;
    }
    return lines;
}


std::string BardFormatter::parseMarkdown(std::string markdown_lyrics)
{

	SongBookUtils::printError("BEFORE AI PARSE");
	SongBookUtils::printError(markdown_lyrics);

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
						- Don't copy the chord lines \
						Other Rules: \
						1. Verse begins with number and dot (e.g., '1. ...'). \
						2. Verses are enumerated (first starts with 1., then 2. ) \
						3. There cannot be verse without an enumeration number. \
						4. Chorus begins with '>' (e.g., '> ...'). \
						5. If the chorus with 'ref', it's replace by '>' \
						6. Preserve lyric words and line breaks exactly. \
						8. Output only the transformed lyrics, no explanations"}},
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
				SongBookUtils::getInstance()->printError(std::format("** {} **", read_json["error"].at("message").get<std::string>()));
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
						SongBookUtils::printError("AFTER AI PARSE");
						SongBookUtils::printError(s);
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

int BardFormatter::addSongPage(nlohmann::json song)
{
	if (song.contains("TITLE") && song.contains("ARTIST") && song.contains("LYRICS")) {
		this->songs.push_back(song);
	}
	else {
		return 1;
	}


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

int LatexListFormatter::addSongPage(nlohmann::json song)
{
	std::ostringstream oss;    
	
	oss << getPrintableString(song["TITLE"])
        << " & "
		<< getPrintableString(song["ARTIST"]) 
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

int LatexListFormatter::generateSongBook(const char* output_dir)
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

int LatexListFormatter::exportSongs(const char* output_dir)
{	
	return 0;
}