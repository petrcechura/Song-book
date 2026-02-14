#include <string>
#include <iostream>
#include <cwchar>
#include "Window.h"

Window::Window(std::string name, int width, int height, int pos_x, int pos_y)
{   
    this->name = name;
    this->width = width;
    this->height = height;
    this->pos_x = pos_x;
    this->pos_y = pos_y;
}

std::wstring Window::utf8_to_wstring(const std::string& s)
{
    std::wstring ws;
    ws.resize(mbstowcs(nullptr, s.c_str(), 0));
    mbstowcs(ws.data(), s.c_str(), ws.size());
    return ws;
}

void Window::Print(const std::string& txt, bool newline)
{
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    int width = max_x - 2;
    scrollok(win, TRUE);

    int y, x;
    getyx(win, y, x);

    if (y < 1) y = 1;
    if (x < 1) x = 1;
    if (x >= max_x - 1) {
        x = 1;
        y++;
    }

    wmove(win, y, x);

    // Convert UTF-8 → wide string
    std::wstring wtxt = utf8_to_wstring(txt);

    size_t i = 0;
    while (i < wtxt.size()) {
        int space = width - (x - 1);
        int chunk = std::min<int>(space, wtxt.size() - i);

        // wide-character output
        waddnwstr(win, wtxt.c_str() + i, chunk);

        i += chunk;
        getyx(win, y, x);

        if (i < wtxt.size()) {
            x = 1;
            y++;
            if (y >= max_y - 1) {
                wscrl(win, 1);
                y = max_y - 2;
            }
            wmove(win, y, x);
        }
    }

    if (newline) {
        x = 1;
        y++;
        if (y >= max_y - 1) {
            wscrl(win, 1);
            y = max_y - 2;
        }
        wmove(win, y, x);
    }

    box(win, 0, 0);
    wrefresh(win);
}

std::string Window::GetString(int max_length)
{   
    if (max_length > 100 || max_length < 1)  {
        std::cerr << "max_length cannot exceed 100 for memory safety reason!" << std::endl;
        return "";
    }

    echo();

    char input[100];

    wgetnstr(this->win, input, 99);   // Read string
    noecho();

    return std::string(input);
}


void Window::Init()
{
    this->win = newwin(this->height, this->width, this->pos_y, this->pos_x);
    box(this->win, 0, 0);
}

void Window::Refresh()
{
    wrefresh(this->win);
}

void Window::Print(const std::string& txt, short background_color, short foreground_color, bool bold)
{   
    init_pair(1, background_color, foreground_color);
    wattron(this->win, COLOR_PAIR(1));

    if (bold)  {
        wattron(win, A_BOLD);
    }

    Print(txt);

    if (bold)  {
        wattroff(win, A_BOLD);
    }
}
void Window::Clear()
{
    wclear(this->win);
}

void Window::Delete()
{
    endwin();
}