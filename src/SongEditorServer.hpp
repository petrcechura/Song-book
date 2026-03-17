#include <string>
#include <vector>

#ifndef SONGEDITORSERVER_H
#define SONGEDITORSERVER_H

class SongEditorServer
{

  public:
    SongEditorServer(std::wstring str, bool* running);
    ~SongEditorServer() {};

    std::wstring Edit(wchar_t c);
    int getLine() { return cursor[0]; };
    int getPos()  { return cursor[1]; };

    static std::vector<std::wstring> str2vec(std::wstring& txt);
    static std::wstring vec2str(std::vector<std::wstring>& vec);

  private:
    /** Cursor position;
     *  <line>, <pos>
     */
    int cursor[2];
    int line_cnt;
    bool* running;
    std::vector<std::wstring>* content;

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