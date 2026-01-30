#ifndef WINDOWSERVER_H
#define WINDOWSERVER_H

#include <string>
#include <ncurses.h>
#include <map>
#include "Window.h"
#include "TaskBase.h"


class WindowServer
{
public:
    ~WindowServer();

    /** Inicialises all ncurses windows and starts a server, thus moving into interractive window */
    void Init();

    /** Waits for an input and periodically refreshes the ncurses interface with tasks */
    void Loop();

    /**  */
    void AddWindow(Window* window);
    /** Adds a `TaskBase` object into the *task container*, thus allow its 
     *  implementation to be part of Command-line application. 
     *
     *  Note that since `TaskBase` is an abstract class, its expected that 
     *  the `task` object will be `TaskBase` derivative, actually */
    int AddTask(TaskBase* task);
protected:
    std::map<std::string, TaskBase*> tasks;
    std::map<std::string, Window*> windows;
};

#endif WINDOWSERVER_H