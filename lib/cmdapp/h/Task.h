#include <string>
#include <iostream>
#include <map>
#include "TaskBase.h"

#ifndef TASK_H
#define TASK_H

/** TODO descr. */
template<class t_cmd>
class Task : public TaskBase
{

  public:
    Task(
        /** A command string for which this task should be called inside main loop */
        std::string cmd,
        /** A reference to a parent class */
        // TODO replace with inner class inside `Cmd`, to obtain access only to database
        t_cmd* parent);
    
    /** This function contains all behaviour the task is supposed to do when called */
    int Start() override;

    void setDescr(std::string d) { this->descr = d; };

    std::string getCmd() override { return cmd; };
    std::string getDescr() { return descr; };

  protected:
    t_cmd* parent;
    std::string cmd;
    std::string descr;
};

template<class t_cmd>
Task<t_cmd>::Task(std::string cmd, t_cmd* parent)
{
    this->cmd = cmd;
    this->parent = parent;
}

template<class t_cmd>
int Task<t_cmd>::Start()
{
    return 0; // Default does nothing
}

#endif
