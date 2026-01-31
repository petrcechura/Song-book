#include <string>
#include <iostream>
#include "Window.h"

Window::Window(std::string name, int width, int height, int pos_x, int pos_y)
{   
    this->name = name;
    this->width = width;
    this->height = height;
    this->pos_x = pos_x;
    this->pos_y = pos_y;
}

void Window::Print(const std::string& txt, bool newline)
{
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    int width = max_x - 2;

    // Allow scrolling when reaching bottom
    scrollok(win, TRUE);

    int y, x;
    getyx(win, y, x);

    // If cursor is on the border, move inside
    if (y < 1) y = 1;
    if (x < 1) x = 1;
    if (x >= max_x - 1) {
        x = 1;
        y++;
    }

    wmove(win, y, x);

    for (size_t i = 0; i < txt.size(); ) {
        // Remaining space on the current line
        int space = width - (x - 1);

        int chunk = std::min<int>(space, txt.size() - i);
        waddnstr(win, txt.c_str() + i, chunk);

        i += chunk;
        getyx(win, y, x);

        // Move to next line if needed
        if (i < txt.size()) {
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


void Window::Init()
{
    this->win = newwin(this->height, this->width, this->pos_y, this->pos_x);
    box(this->win, 0, 0);
}

void Window::Refresh()
{
    wrefresh(this->win);
}

void Window::Print(const std::string& txt, textColor_t background_color, textColor_t foreground_color, bool bold)
{   
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