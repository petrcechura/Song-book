#ifndef WTASK_H
#define WTASK_H

#include <map>
#include <Window.h>
#include <string>
#include <functional>
#include "TaskBase.h"
#include "WindowServer.h"
#include <iostream>

template<class t_server>
class WTask : public TaskBase
{
public:
    WTask(
        /** A command string for which this task should be called inside main loop */
        std::string name,
        /** A reference to a parent class */
        t_server* parent,
        /** A description to be printed when calling `help` */
        std::string description="no description provided");
    /**  */
    virtual int Execute(char command) override;
    virtual std::string GetName() override;
    virtual std::string GetDescription() override;
    virtual int ParseCommand(std::string cmd_line) override;
    
    void AddWindow(Window* window);

protected:
    std::map<char, std::function<void(void)>> commands;
    std::map<std::string, Window*> windows;

    std::string name;
    std::string description;
    t_server* parent;
};

template<class t_server>
WTask<t_server>::WTask(std::string cmd, t_server* parent, std::string descr)
{
    this->name = cmd;
    this->parent = parent;
    this->description = descr;
}

template<class t_server>
int WTask<t_server>::Execute(char command) 
{
    this->commands[command]();

    return 0;
}

template<class t_server>
std::string WTask<t_server>::GetName() 
{
    return this->name;
}

template<class t_server>
std::string WTask<t_server>::GetDescription()
{
    return this->description;
}

template<class t_server>
int WTask<t_server>::ParseCommand(std::string cmd_line)
{
    return 0;
}

template<class t_server>
void WTask<t_server>::AddWindow(Window* window)
{
    windows[window->GetName()] = window;
}

#endif WTASK_H