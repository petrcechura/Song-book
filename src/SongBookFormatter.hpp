

/** This virtual class shall provide an interface for a Gather and Latex task.
    - takes chords in plain text as an argument and calls an AI to parse to **implementation-depedent** format (bard, latex)
    - provides set of methods to convert this text to a pdf
    - creates entire songbook
    
    - A visitor pattern*/
class SongBookFormatter
{
public:
    virtual int generateSongBook(const char* output_file = "") = 0;
    virtual int parseMarkdown(const char* title,
                              const char* artist,
                              const char* markdown_lyrics) = 0;
    virtual bool checkSanity() = 0;
    virtual void clearPages() = 0;
}

class LatexListFormatter : public SongBookFormatter
{
    virtual int generateSongBook(const char* output_file = "") override;
    virtual int parseMarkdown(const char* title,
                              const char* artist,
                              const char* markdown_lyrics) override;
    virtual bool checkSanity() override;
    virtual void clearPages() override;
}

class BardFormatter : public SongBookFormatter
{
    
}
