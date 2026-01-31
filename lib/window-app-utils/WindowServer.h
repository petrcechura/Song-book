#ifndef WINDOWSERVER_H
#define WINDOWSERVER_H

#include <string>
#include <ncurses.h>
#include <map>
#include "Window.h"
#include "json.hpp"
#include "TaskBase.h"


class WindowServer
{
public:
    WindowServer();
    WindowServer(nlohmann::json _config);
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
    int executeTaskString(std::string task_string);

protected:
    std::map<std::string, TaskBase*> tasks;
    std::map<std::string, Window*> windows;
    char EXIT_CHAR = 27; // ESC character

    virtual void StartHook();
    virtual void StopHook();
};

#endif