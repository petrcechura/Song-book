#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>
#include <curl/curl.h>
#include "utf8.h"
#include <format>
#include <filesystem>
#include <podofo/podofo.h>
#include <fstream>
#include <map>
#include "json.hpp"
#include "SongBookFormatter.h"
#include "SongBookUtils.h"

int BardFormatter::exportSongs(const char* output_dir)
{	

	// create output directory
	std::filesystem::create_directory(output_dir);

	// create directory for songs
	std::string songs_dir = std::format("{}/songs", output_dir);
	std::filesystem::create_directory(songs_dir);

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

	// init directory with bard tool
    std::string bard_tool = SongBookUtils::getConfigItem("paths/bard_command", "");
    std::string s = SongBookUtils::execSystemCommand(std::format("(cd {} && {} init)", output_dir, bard_tool).c_str());
	SongBookUtils::printError(std::format("(cd {} && {} init)", output_dir, bard_tool));
	SongBookUtils::printError(s);
	
	// create TOML file
	std::ofstream tomlFile(std::format("{}/bard.toml", output_dir).c_str());
	tomlFile << toml_oss.str();	
	tomlFile.close();

    if (bard_tool.empty())  {
      return 1;
    }

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
  
 
  std::string s = SongBookUtils::execSystemCommand(std::format("(cd {} && {} make)", output_dir, bard_tool).c_str());
  SongBookUtils::printError(s);

  // TODO create symbolink link to work?

  return 0;
}

void BardFormatter::createBooklet(const char* def_songbook_path,
                           		  const char* front_page_path,
                           		  const char* back_page_path,
                           		  int pages_per_shuttle)
{

}

std::string BardFormatter::processSingleChordLine(const std::string& chord_line)
{
	std::ostringstream s;

	bool chord = false;
	for (const auto c : chord_line) {
		if (c == ' ')  {
			s << (chord ? "`" : "") << c;
			chord = false;
		}
		else if (c != ' ')  {
			s << (chord ? "" : "`") << c;
			chord = true;
		}
	}

	return s.str();
}

std::string BardFormatter::processTextWithChords(const std::string& chord_line, const std::string& text_line)
{
	std::ostringstream buffer;
	
	std::map<int, std::string> chords;
	int track_i = 0;
	bool processing_chord = false;
	int i = 0;
	for (const auto& c : chord_line) {

		// chord detected
		if (c!=' ')  {
			buffer << c;
			if (!processing_chord) {
				track_i = i;
				processing_chord = true;
			}

		// empty char
		} else {
			if (processing_chord)  {
				chords[track_i] =  buffer.str();
				processing_chord = false;
				buffer.str("");
			}
		}
		i++;
	}

	std::ostringstream s;
	i = 0;
	for (auto it = text_line.begin(); it != text_line.end(); ) {
		if (chords.contains(i))  {
			s << "`" << chords[i] << "`";
		}
		uint32_t cp = utf8::next(it, text_line.end());
		utf8::append(cp, std::ostream_iterator<char>(s));
		i++;
	}

	return s.str();
}
std::string BardFormatter::processTextLine(const std::string& text_line)
{	
	return text_line;
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

std::string LatexListFormatter::processSingleChordLine(const std::string& chord_line)
{

}
std::string LatexListFormatter::processTextWithChords(const std::string& chord_line, const std::string& text_line)
{

}
std::string LatexListFormatter::processTextLine(const std::string& text_line)
{
	
}

void LatexListFormatter::createBooklet(const char* def_songbook_path,
                               		   const char* front_page_path,
                               		   const char* back_page_path,
                               		   int pages_per_shuttle)
{
	
}