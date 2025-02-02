#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "../h/LatexTask.h"
#include "../../SongBookApp.h"

int LatexTask::Start()
{
  using json_t = nlohmann::json;

  const int SONGS_PER_PAGE = 25;

  const char* title_page = R"(
    \maketitle
  )";

  const char* header = R"(
    \documentclass[17pt]{memoir}
    \usepackage[czech]{babel}
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

  std::ofstream file("database.tex");

  file << header;
  file << title_page;
  file << table_begin;

	json_t data = parent->getDatabase()->getJson();
  for (int i = 0; i < parent->getDatabase()->getSongCount(); i++)  {

    const auto& song = parent->getDatabase()->getSong(i);
    file << i << " & " << song->getName(true) << " & " << song->getAuthor(true) << R"( \\)";

    if (i % SONGS_PER_PAGE == 0 && i > 0 && i < parent->getDatabase()->getSongCount()-1)  {
      file << table_end;
      file << table_begin;
    }
  }
  file << table_end;
  file << ending;
	
	file.close();

  system("pdflatex database.tex");

	return 0;
}
