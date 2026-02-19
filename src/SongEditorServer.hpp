#include <string>

#ifndef SONGEDITORSERVER_H
#define SONGEDITORSERVER_H

class SongEditorServer
{

  public:
    SongEditorServer(std::wstring* content, bool* running);
    ~SongEditorServer() {};

    void Edit(wchar_t c);

  private:
    /** Cursor position */
    int cursor;
    bool* running;
    std::wstring* content;

    void deleteLine();
    void deleteChar();
    void deleteChords();
    void insertChar(wchar_t c);
    void insertChord();
    void transpose(int i);
    void exitEditor();
};

#endif