#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <ncurses.h>
#include <cwchar>

class Window
{

public:

private:
    int width;
    int height;
    int pos_x;
    int pos_y;
    std::string name;
    WINDOW* win;

public:
    Window(std::string name, int width, int height, int pos_x, int pos_y);

    std::wstring utf8_to_wstring(const std::string& s);

    void Print(const std::string& txt, bool newline=true);
    void Print(const std::string& txt, short background_color, short foreground_color, bool bold = false);
    void Clear();

    void Init();
    void Refresh();

    void Delete();

    int GetPosX() { return pos_x; };
    int GetPosY() { return pos_y; };
    int GetWidth() { return width; };
    int GetHeight() { return height; };

    std::string GetName() { return name; };
};

#endif