#ifndef LATEXSONGBOOKFORMATTER_H
#define LATEXSONGBOOKFORMATTER_H

#include <string>
#include <vector>
#include "json.hpp"
#include "SongBookFormatter.h"

/** LaTeX implementation of SongBookFormatter that renders each song as its
 *  own page: title, artist, numbered verses, chorus, and inline chords.
 *
 *  Expected internal format of song["LYRICS"] (same canonical format used
 *  across formatters):
 *    - verses start with "N. " (e.g. "1. ", "2. ")
 *    - chorus starts with "> "
 *    - a capo / performance note starts with ":: " and is rendered in italics
 *    - chords are inlined right before the word they belong to, wrapped in
 *      backticks, e.g. "I will `Ami`be there"
 *    - blocks (verses / chorus / notes) are separated by a blank line
 *
 *  This is a basic implementation: chords attached to a specific word are
 *  rendered as a bold superscript right before that word. The verse/chorus
 *  number, and any chord-only line preceding a verse (e.g. intro chords
 *  played before the vocals start), are placed in a fixed-width left column
 *  so they never crowd the lyric text itself; the lyrics sit in their own
 *  column to the right.
 *
 *  Two-phase contract (same as BardFormatter):
 *    - exportSongs(output_dir)   consumes this->songs and writes the full
 *                                songbook.tex to output_dir.
 *    - generateSongBook(output_dir) does NOT read this->songs; it just
 *                                   compiles the already-written
 *                                   output_dir/songbook.tex with pdflatex.
 *  Call exportSongs() before generateSongBook().
 */
class LatexSongbookFormatter : public SongBookFormatter
{
protected:
    nlohmann::json songs;

    enum class BlockType { VERSE, CHORUS, NOTE, PLAIN };

    struct LyricBlock
    {
        BlockType type;
        std::string number;          // verse number; empty for chorus/note/plain
        std::vector<std::string> lines;
        std::string leadChords;      // raw backtick-marked chords merged in from
                                      // a preceding chord-only line (e.g. intro
                                      // chords played before this verse starts),
                                      // rendered in the left label column instead
                                      // of inline with the lyric text.
    };

    // LaTeX document skeleton
    const char* preamble = R"(\documentclass[12pt]{article}
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage[margin=2cm]{geometry}
\usepackage{array}
\pagenumbering{gobble}

% Chord placed right before the syllable/word it belongs to
\newcommand{\chordmark}[1]{\textsuperscript{\textbf{#1}}}

\begin{document})";

    const char* ending = R"(
\end{document})";

    std::string escapeLatex(const std::string& text) const;
    std::string renderLineWithChords(const std::string& line) const;
    std::string formatChordList(const std::string& raw) const;
    std::vector<LyricBlock> parseLyrics(const std::string& lyrics) const;
    std::string renderBlock(const LyricBlock& block) const;

public:
    LatexSongbookFormatter() {};
    ~LatexSongbookFormatter() {};

    virtual int generateSongBook(const char* output_dir = "") override;
    virtual int exportSongs(const char* output_dir) override;
    virtual int addSongPage(nlohmann::json song) override;
    virtual bool checkSanity() override;
    virtual void clearPages() override;

    virtual std::string processSingleChordLine(const std::string& chord_line) override;
    virtual std::string processTextWithChords(const std::string& chord_line, const std::string& text_line) override;
    virtual std::string processTextLine(const std::string& text_line) override;

    virtual void createBooklet(const char* def_songbook_path,
                               const char* front_page_path,
                               const char* back_page_path,
                               int pages_per_shuttle) override;
};

#endif
