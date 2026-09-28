#include <sstream>
#include <fstream>
#include <regex>
#include <format>
#include <filesystem>
#include <ostream>
#include <map>
#include "utf8.h"
#include "LatexSongbookFormatter.h"
#include "SongBookUtils.h"

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

std::string LatexSongbookFormatter::escapeLatex(const std::string& text) const
{
	std::ostringstream out;
	for (char c : text) {
		switch (c) {
			case '\\': out << "\\textbackslash{}"; break;
			case '&':  out << "\\&"; break;
			case '%':  out << "\\%"; break;
			case '$':  out << "\\$"; break;
			case '#':  out << "\\#"; break;
			case '_':  out << "\\_"; break;
			case '{':  out << "\\{"; break;
			case '}':  out << "\\}"; break;
			case '~':  out << "\\textasciitilde{}"; break;
			case '^':  out << "\\textasciicircum{}"; break;
			default:   out << c;
		}
	}
	return out.str();
}

// Turns a single lyric line containing `chord`-marked chords into a LaTeX
// line, escaping plain text and wrapping chords in \chordmark{}.
std::string LatexSongbookFormatter::renderLineWithChords(const std::string& line) const
{
	std::ostringstream out;
	size_t pos = 0;

	while (pos < line.size()) {
		size_t open = line.find('`', pos);
		if (open == std::string::npos) {
			out << escapeLatex(line.substr(pos));
			break;
		}

		// plain text before the chord
		out << escapeLatex(line.substr(pos, open - pos));

		size_t close = line.find('`', open + 1);
		if (close == std::string::npos) {
			// unterminated backtick - treat the rest as plain text
			out << escapeLatex(line.substr(open));
			break;
		}

		std::string chord = line.substr(open + 1, close - open - 1);
		out << "\\chordmark{" << escapeLatex(chord) << "}";
		pos = close + 1;
	}

	return out.str();
}

// Extracts chord names from backtick-marked text (ignoring the words they'd
// normally attach to) and joins them as plain text, for display in the left
// label column instead of as inline superscripts.
std::string LatexSongbookFormatter::formatChordList(const std::string& raw) const
{
	std::ostringstream out;
	size_t pos = 0;
	bool first = true;

	while (pos < raw.size()) {
		size_t open = raw.find('`', pos);
		if (open == std::string::npos) break;

		size_t close = raw.find('`', open + 1);
		if (close == std::string::npos) break;

		std::string chord = raw.substr(open + 1, close - open - 1);
		if (!first) {
			out << " ";
		}
		out << escapeLatex(chord);
		first = false;
		pos = close + 1;
	}

	return out.str();
}

// Splits raw LYRICS text into verse / chorus / note / plain blocks.
std::vector<LatexSongbookFormatter::LyricBlock> LatexSongbookFormatter::parseLyrics(const std::string& lyrics) const
{
	std::vector<LyricBlock> blocks;
	std::istringstream iss(lyrics);
	std::string line;

	// A verse marker is any non-whitespace label followed by a literal dot,
	// e.g. "1.", "2.", or a custom label like "*.". Chorus ("> ") and capo
	// notes ("::") are checked first since they're more specific patterns -
	// otherwise the generic "label." pattern could swallow them.
	static const std::regex chorus_re(R"(^\s*>\s*(.*)$)");
	static const std::regex note_re(R"(^\s*::\s*(.*)$)");
	static const std::regex verse_re(R"(^\s*(\S+)\.\s*(.*)$)");

	LyricBlock* current = nullptr;

	auto isBlank = [](const std::string& s) {
		return s.find_first_not_of(" \t\r") == std::string::npos;
	};

	while (std::getline(iss, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}

		if (isBlank(line)) {
			// A blank line is just a visual separator in the source text. It
			// does NOT end the current block: an unmarked line that follows
			// (even after a blank line) still belongs to whatever block is
			// currently open - only an explicit verse/chorus/note marker
			// starts a new block. This only matters before the very first
			// marker in the song, where there is no open block yet.
			continue;
		}

		std::smatch m;
		if (std::regex_match(line, m, verse_re)) {
			blocks.push_back({ BlockType::VERSE, m[1].str(), {}, "" });
			current = &blocks.back();
			if (!m[2].str().empty()) {
				current->lines.push_back(m[2].str());
			}
		}
		else if (std::regex_match(line, m, chorus_re)) {
			blocks.push_back({ BlockType::CHORUS, "", {}, "" });
			current = &blocks.back();
			if (!m[1].str().empty()) {
				current->lines.push_back(m[1].str());
			}
		}
		else if (std::regex_match(line, m, note_re)) {
			blocks.push_back({ BlockType::NOTE, "", {}, "" });
			current = &blocks.back();
			if (!m[1].str().empty()) {
				current->lines.push_back(m[1].str());
			}
		}
		else if (current != nullptr) {
			current->lines.push_back(line);
		}
		else {
			// text appearing outside of any verse/chorus/note marker
			blocks.push_back({ BlockType::PLAIN, "", { line }, "" });
			current = &blocks.back();
		}
	}

	// Merge any chord-only PLAIN blocks (e.g. intro chords played before a
	// verse starts) forward into the next verse/chorus block's left-hand
	// label column, instead of gluing them into the lyric text or leaving
	// them as a standalone dangling paragraph.
	for (size_t i = 0; i < blocks.size(); ) {
		if (blocks[i].type == BlockType::PLAIN) {
			size_t j = i + 1;
			if (j < blocks.size() && (blocks[j].type == BlockType::VERSE || blocks[j].type == BlockType::CHORUS)) {
				std::string chordsJoined;
				for (size_t k = 0; k < blocks[i].lines.size(); ++k) {
					if (k > 0) chordsJoined += " ";
					chordsJoined += blocks[i].lines[k];
				}

				if (blocks[j].leadChords.empty()) {
					blocks[j].leadChords = chordsJoined;
				}
				else {
					blocks[j].leadChords = chordsJoined + " " + blocks[j].leadChords;
				}

				blocks.erase(blocks.begin() + i);
				continue;
			}
		}
		++i;
	}

	return blocks;
}

std::string LatexSongbookFormatter::renderBlock(const LyricBlock& block) const
{
	std::ostringstream out;

	// Capo / performance notes: a standalone italic paragraph, not part of
	// the verse/chorus column layout.
	if (block.type == BlockType::NOTE) {
		out << "\\noindent\\textit{";
		for (size_t i = 0; i < block.lines.size(); ++i) {
			out << renderLineWithChords(block.lines[i]);
			if (i + 1 < block.lines.size()) {
				out << "\\newline ";
			}
		}
		out << "}\n\\vspace{0.3em}\n\n";
		return out.str();
	}

	// Rare fallback: chord-only or unmarked text that never got merged into
	// a following verse/chorus (e.g. it's the very last block in the song).
	if (block.type == BlockType::PLAIN) {
		out << "\\noindent ";
		for (size_t i = 0; i < block.lines.size(); ++i) {
			out << renderLineWithChords(block.lines[i]);
			out << (i + 1 < block.lines.size() ? "\\newline " : "\n");
		}
		if (!block.leadChords.empty()) {
			out << formatChordList(block.leadChords) << "\n";
		}
		out << "\\vspace{0.5em}\n\n";
		return out.str();
	}

	// Verse / chorus: a two-column layout. The left column holds the verse
	// number (or "R." for chorus); the lyrics live in the right column, with
	// their own inline \chordmark{} chords preserved. Any lead-in chords
	// (e.g. an intro played before the verse starts) are shown as their own
	// small full-width line above the block, rather than stacked inside the
	// narrow 2.6em number column - a long chord list would otherwise be
	// forced one chord per line, and even a short one can end up visually
	// detached if it makes the label cell taller than the row next to it.
	std::string label = (block.type == BlockType::VERSE) ? (block.number + ".") : std::string("R.");
	std::string labelCell = "\\textbf{" + label + "}";
	std::string introLine;

	if (!block.leadChords.empty()) {
		introLine = "\\noindent{\\small " + formatChordList(block.leadChords) + "}\\par\\vspace{0.2em}\n";
	}

	out << introLine;
	out << "\\noindent\\begin{tabular}[t]{@{}>{\\raggedright\\arraybackslash}p{2.6em}@{\\hspace{0.4em}}p{\\dimexpr\\linewidth-3em\\relax}@{}}\n";

	if (block.lines.empty()) {
		out << labelCell << " & \\\\\n";
	}
	else {
		for (size_t i = 0; i < block.lines.size(); ++i) {
			out << (i == 0 ? labelCell : std::string(""));
			out << " & " << renderLineWithChords(block.lines[i]);
			out << (i + 1 < block.lines.size() ? " \\\\\n" : "\n");
		}
	}

	out << "\\end{tabular}\n\\vspace{0.5em}\n\n";

	return out.str();
}

// ---------------------------------------------------------------------
// SongBookFormatter interface
// ---------------------------------------------------------------------

int LatexSongbookFormatter::exportSongs(const char* output_dir)
{
	// Mirrors BardFormatter's contract: exportSongs() is the phase that
	// consumes this->songs and persists everything to disk. generateSongBook()
	// deliberately does NOT touch this->songs, since callers may clear the
	// in-memory song list (or use a different object) between the two calls.
	std::string dir = (output_dir != nullptr && std::string(output_dir).size() > 0) ? output_dir : ".";
	std::filesystem::create_directories(dir);

	std::ostringstream doc;
	doc << preamble << "\n";

	std::string book_title = SongBookUtils::getInstance()->getConfigItem("songbook/title");
	std::string book_subtitle = SongBookUtils::getInstance()->getConfigItem("songbook/subtitle");

	if (!book_title.empty()) {
		doc << "\\begin{center}\n{\\Huge \\textbf{" << escapeLatex(book_title) << "}}\\\\[0.5em]\n";
		if (!book_subtitle.empty()) {
			doc << "{\\Large " << escapeLatex(book_subtitle) << "}\n";
		}
		doc << "\\end{center}\n\\newpage\n\n";
	}

	// Contents page: one hyperlinked, page-numbered entry per song, filled
	// in automatically from the \addcontentsline calls below. pdflatex is
	// run twice by generateSongBook() so the page numbers resolve correctly.
	doc << "\\tableofcontents\n\\newpage\n\n";

	for (const auto& song : this->songs) {
		std::string title = SongBookUtils::getInstance()->sql2txt(song["TITLE"]);
		std::string artist = SongBookUtils::getInstance()->sql2txt(song["ARTIST"]);
		std::string lyrics = SongBookUtils::getInstance()->sql2txt(song["LYRICS"]);

		doc << "\\phantomsection\n";
		doc << "\\addcontentsline{toc}{section}{" << escapeLatex(title) << " -- " << escapeLatex(artist) << "}\n";

		doc << "\\noindent{\\Large\\bfseries " << escapeLatex(title) << "}\\\\\n";
		doc << "{\\normalsize\\itshape " << escapeLatex(artist) << "}\n";
		doc << "\\vspace{1em}\n\n";

		auto blocks = parseLyrics(lyrics);
		for (const auto& block : blocks) {
			doc << renderBlock(block);
		}

		doc << "\\newpage\n\n";
	}

	doc << ending;

	std::filesystem::path tex_path = std::filesystem::path(dir) / "songbook.tex";
	std::ofstream file(tex_path);
	file << doc.str();
	file.close();

	return 0;
}

int LatexSongbookFormatter::generateSongBook(const char* output_dir)
{
	std::string dir = (output_dir != nullptr && std::string(output_dir).size() > 0) ? output_dir : ".";
	std::filesystem::path tex_path = std::filesystem::path(dir) / "songbook.tex";

	if (!std::filesystem::exists(tex_path)) {
		// exportSongs() must run first to produce songbook.tex
		return 2;
	}

	// run pdflatex twice so layout settles
	std::string cmd = std::format(
		"(cd {} && pdflatex -interaction=nonstopmode songbook.tex && pdflatex -interaction=nonstopmode songbook.tex)",
		dir);
	std::string result = SongBookUtils::execSystemCommand(cmd.c_str());
	SongBookUtils::printError(result);

	return 0;
}

int LatexSongbookFormatter::addSongPage(nlohmann::json song)
{
	if (song.contains("TITLE") && song.contains("ARTIST") && song.contains("LYRICS")) {
		this->songs.push_back(song);
		return 0;
	}

	return 1;
}

bool LatexSongbookFormatter::checkSanity()
{
	std::string result = SongBookUtils::execSystemCommand("which pdflatex");
	return !result.empty();
}

void LatexSongbookFormatter::clearPages()
{
	this->songs.clear();
}

// ---------------------------------------------------------------------
// Chord preprocessing - mirrors the canonical backtick-encoding used to
// store chords/lyrics uniformly across formatter implementations.
// ---------------------------------------------------------------------

std::string LatexSongbookFormatter::processSingleChordLine(const std::string& chord_line)
{
	std::ostringstream s;

	bool chord = false;
	for (const auto c : chord_line) {
		if (c == ' ') {
			s << (chord ? "`" : "") << c;
			chord = false;
		}
		else {
			s << (chord ? "" : "`") << c;
			chord = true;
		}
	}

	return s.str();
}

std::string LatexSongbookFormatter::processTextWithChords(const std::string& chord_line, const std::string& text_line)
{
	std::ostringstream buffer;

	std::map<int, std::string> chords;
	int track_i = 0;
	bool processing_chord = false;
	int i = 0;
	for (const auto& c : chord_line) {
		if (c != ' ') {
			buffer << c;
			if (!processing_chord) {
				track_i = i;
				processing_chord = true;
			}
		}
		else {
			if (processing_chord) {
				chords[track_i] = buffer.str();
				processing_chord = false;
				buffer.str("");
			}
		}
		i++;
	}
	if (processing_chord) {
		chords[track_i] = buffer.str();
	}

	std::ostringstream s;
	i = 0;
	for (auto it = text_line.begin(); it != text_line.end();) {
		if (chords.contains(i)) {
			s << "`" << chords[i] << "`";
		}
		uint32_t cp = utf8::next(it, text_line.end());
		utf8::append(cp, std::ostream_iterator<char>(s));
		i++;
	}

	return s.str();
}

std::string LatexSongbookFormatter::processTextLine(const std::string& text_line)
{
	return text_line;
}

void LatexSongbookFormatter::createBooklet(const char* def_songbook_path,
											const char* front_page_path,
											const char* back_page_path,
											int pages_per_shuttle)
{
	// not implemented yet
}
