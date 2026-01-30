#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <ncurses.h>

class Window
{
private:
    int width;
    int height;
    int pos_x;
    int pos_y;
    std::string name;
    WINDOW* win;

    typedef enum {
        BLACK,
        BLUE,
        WHITE,
        CYAN,
        YELLOW
    } textColor_t;
public:
    Window(std::string name, int width, int height, int pos_x, int pos_y);


    void Print(const std::string& txt, bool newline=true);
    void Print(const std::string& txt, textColor_t background_color, textColor_t foreground_color, bool bold = false);
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

#endif WINDOW_H