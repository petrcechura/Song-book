#include <ncurses.h>
#include <string>
#include "WindowServer.h"
#include "TaskBase.h"
#include "Window.h"
#include <iostream>


WindowServer::~WindowServer()
{
    for (auto const& [name, window] : windows)  {
        window->Delete();
        delete window; 
    }

    for (auto const& [name, task] : tasks)  {
        delete task; 
    }
}

void WindowServer::Init()
{   
    initscr();
    noecho();
    cbreak();

    for (auto const& [name, window] : windows)  {
        window->Init();
        refresh();
        window->Refresh(); 
    }
}

void WindowServer::Loop()
{
    int c = 'x';

    while(c != 27) {
        c = getch();

        for (auto const& [name, task] : tasks)  {
            task->Execute(c);
        }
    }

}

void WindowServer::AddWindow(Window* window)
{
    this->windows[window->GetName()] = window;
}


int WindowServer::AddTask(TaskBase* task)
{
    this->tasks[task->GetName()] = task;

    return 0;
}