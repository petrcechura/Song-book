#include <string>
#include <iostream>
#include <map>
#include "TaskBase.h"

#ifndef TASK_H
#define TASK_H

/** Default `TaskBase` implementation and a natural top-class to all `Task` subclasses. This
 *  class provides default method implementations for `TestBase`
 *  and introduces `template` methods, used in top-level class `CmdApp`
 */
template<class t_cmd>
class Task : public TaskBase
{

  public:
    Task(
        /** A command string for which this task should be called inside main loop */
        std::string cmd,
        /** A reference to a parent class */
        t_cmd* parent,
        /** A description to be printed when calling `help` */
        std::string descr="no description provided");
    
    /** This method shall be overriden and contains an implementation of a specific `Task`,
     *  thus shall "do everything a task is supposed to do"
     */
    int Start() override;
    
    /** An aux. setter for a description */
    void setDescr(std::string d) { this->descr = d; };
    
    /** A getter for `cmd` string */
    std::string getCmd() override { return cmd; };
    /** A getter for a description */
    std::string getDescr() override { return descr; };

  protected:
    t_cmd* parent;
    std::string cmd;
    std::string descr;
};

template<class t_cmd>
Task<t_cmd>::Task(std::string cmd, t_cmd* parent, std::string descr)
{
    this->cmd = cmd;
    this->parent = parent;
    this->descr = descr;
}

template<class t_cmd>
int Task<t_cmd>::Start()
{
    return 0; // Default does nothing
}

#endif
