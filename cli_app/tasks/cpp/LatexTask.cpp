#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <format>
#include "LatexTask.h"
#include "SongBookApp.h"
#include <map>
#include "json.hpp"

int LatexTask::executeCommand()
{

  const int SONGS_PER_PAGE = 25;

  const char* title_page = R"(
    \maketitle
  )";

  const char* header = R"(
    \documentclass[17pt]{memoir}
    % \usepackage[czech]{babel}
    \usepackage{bookman}
    \usepackage[none]{hyphenat}
    \usepackage[T1]{fontenc}
    \fontfamily{georgia}\selectfont
    \usepackage{csvsimple}
    \pagenumbering{gobble}
    \usepackage{booktabs}    
    \usepackage{titling}
    \usepackage[utf8]{inputenc}  
    \usepackage{calc}
    \usepackage{layout}
    \usepackage{graphicx}  
    \usepackage{amsmath, amssymb} 
    \usepackage{ marvosym }
    \usepackage{threeparttable}

    % Define custom title page
    \pretitle{\begin{center}\Huge\bfseries} % Format title
    \posttitle{\par\end{center}}
    \preauthor{\begin{center}\Large\itshape} % Format author
    \postauthor{\par\end{center}}
    \predate{\begin{center}\normalsize} % Format date
    \postdate{\par\end{center}}

    \title{Písničkovník}
    \author{Iči}
    
    \begin{document})";

  const char* table_begin = R"(
    \begin{table}[htbp]
    \centering
    \begin{tabular}{p{1.2cm}|p{8.5cm}|p{6cm}}
    \toprule
    \textbf{ID} & \textbf{Název písně} & \textbf{Autor písně} \\
    \midrule)";

  const char* table_end = R"(
    \bottomrule
    \end{tabular} 
    \end{table} 
    \pagebreak)";
  
  const char* ending = R"(
    \end{document})";

  std::filesystem::path tex_path = out_path;
  tex_path.replace_extension("tex").filename();
  std::ofstream file(tex_path);

  file << header;
  file << title_page;
  file << table_begin;

	nlohmann::json data = parent->getDatabase()->getJson();

  int i = 0;
  std::string title;
	std::string artist;
	std::string id;
  for (const auto& song : data)  {
    title = song.count("TITLE") ? song.at("TITLE") : "NULL";
    artist = song.count("ARTIST") ? song.at("ARTIST") : "NULL";
    file  << i 
          << " & " << getPrintableString(title)
          << " & " << getPrintableString(artist) 
          << R"( \\)";

    if (i % SONGS_PER_PAGE == 0 && i > 0 && i < parent->getDatabase()->getSongCount()-1)  {
      file << table_end;
      file << table_begin;
    }

    i++;
  }
  file << table_end;
  file << ending;
	
	file.close();

  
  system(std::format("pdflatex -output-directory={} {} && clear", 
            (this->out_path.has_parent_path() ?  this->out_path.parent_path().string() : "."),
            tex_path.string()
          ).c_str());

	return 0;
}


std::string LatexTask::getPrintableString(std::string str)
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

void LatexTask::setOutPath(std::string out_path) 
{
  try {
    this->out_path = std::filesystem::path(out_path);

    this->out_path.replace_extension("pdf");
  }
  catch(const std::exception& e)  {}
};
