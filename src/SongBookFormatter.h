#ifndef SONGBOOKFORMATTER_H
#define SONGBOOKFORMATTER_H

#include <string>
#include <sstream>
#include <curl/curl.h>
#include "json.hpp"



/** This virtual class shall provide an interface for a Gather and Latex task.
    - takes chords in plain text as an argument and calls an AI to parse to **implementation-depedent** format (bard, latex)
    - provides set of methods to convert this text to a pdf
    - creates entire songbook
    
    - A visitor pattern*/
class SongBookFormatter
{
protected:
    std::string ai_api_key;
    std::string ai_model;
    std::vector<std::string> songs;

    // Callback to collect the response into a string
    static size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

public:

    SongBookFormatter() {};
    ~SongBookFormatter() {};

    /** Generates a complete songbook under the `output_file` path, including title page.
     *
     *  Song pages to this complete songbook shall be added via `addSongPage` method.
     */
    virtual int generateSongBook(const char* output_file = "") = 0;
    
    /** 
     * Parses markdown syntax into `SongBookFormatter` specific syntax.
     *
     * Markdown syntax is universal syntax that is parsed via `pandoc` tool by `GatherTask`.
     * This method converts this syntax to implementation specific syntax, so songbook may be properly generated.
     *
     * Converted lyrics are not complete song page; such page shall be generated only via `addSongPage`.
     */
    virtual std::string parseMarkdown(std::string markdown_lyrics) = 0;

    /** Adds a song into temporary database, so songbook can be generated. 
        @param title: title of a song
        @param artist: artist of a song
        @param lyrics: lyrics of a song in `implementation-specific` syntax
      */
    virtual int addSongPage(std::string title, std::string artist, std::string lyrics) = 0;

    /** Checks whether this system has all dependencies the implementation requires */
    virtual bool checkSanity() = 0;
    
    /** Clears temporary database.
     *
     *  Shall be called after songbook generation.
     */
    virtual void clearPages() = 0;

    virtual void setAiApiKey(std::string ai_api_key)
    {
        this->ai_api_key = ai_api_key;
    }

    virtual void setAiModel(std::string ai_model)
    {
        this->ai_model = ai_model;
    }
};

class BardFormatter : public SongBookFormatter
{
protected:
    std::vector<std::string> songs;
public:
    virtual int generateSongBook(const char* output_file = "") override;
    virtual std::string parseMarkdown(std::string markdown_lyrics) override;
    virtual int addSongPage(std::string title, std::string artist, std::string lyrics);
    virtual bool checkSanity() override;
    virtual void clearPages() override;
};

class LatexListFormatter : public SongBookFormatter
{
protected:
    std::vector<std::string> songs;
    const int SONGS_PER_PAGE = 25;

    const char* title_page = R"(
        \maketitle)";

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
        \begin{tabular}{p{8.5cm}|p{6cm}}
        \toprule
        \textbf{Název písně} & \textbf{Autor písně} \\
        \midrule)";

  const char* table_end = R"(
        \bottomrule
        \end{tabular} 
        \end{table} 
        \pagebreak)";
  
  const char* ending = R"(
        \end{document})";

    std::string getPrintableString(std::string str);
public:
    virtual int generateSongBook(const char* output_file = "") override;
    virtual std::string parseMarkdown(std::string markdown_lyrics) override;
    virtual int addSongPage(std::string title, std::string artist, std::string lyrics);
    virtual bool checkSanity() override;
    virtual void clearPages() override;
};

#endif