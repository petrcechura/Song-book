#include <string>
#include <vector>

#ifndef SONGEDITORSERVER_H
#define SONGEDITORSERVER_H

class SongEditorServer
{

  public:
    //SongEditorServer(std::vector<std::wstring>* content, bool* running);
    SongEditorServer(std::wstring* content, bool* running);
    ~SongEditorServer() {};

    void Edit(wchar_t c);

  private:
    /** Cursor position */
    int cursor;
    int line;
    bool* running;
    //std::vector<std::wstring>* content;
    std::wstring* content;

    void deleteChar();
    void deleteChords();
    void insertChar(wchar_t c);
    void insertChord();
    void transpose(int i);
    void exitEditor();
    void moveRight();
    void moveLeft();
    void moveUp();
    void moveDown();
    void moveAt(int line, int pos);
    void createLine();
    void deleteLine();
};

#endif