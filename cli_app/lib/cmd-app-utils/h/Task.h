#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "TaskBase.h"
#include <sstream>

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
    
    /** 
     *  Starts the interactive (CLI) approach to obtain all required arguments for task execution.
     */
    int startInteractive() override;
    
    /** An aux. setter for a description */
    void setDescr(std::string d) { this->descr = d; };
    
    /** A getter for `cmd` string */
    std::string getCmd() override { return cmd; };
    /** A getter for a description */
    std::string getDescr() override { return descr; };
    
    /** Assigns new argument for a task to be used after the command name to specify additional actions
     *  @param arg
     *  Specifies the argument name. Either -<arg> or --<arg> can be later used.
    */
    int addArg(std::string arg);
    
    /** 
     *  Executes this task. For execution, arguments obtained via `Start` (interactive) or `parseCommand` (non-interactive) are used.
     */
    int executeCommand();
    
    /** Attempts to parse a command line string (arguments separation) and stores its contents into this class.
     * 
     *  Returns 0 when cmd string has successfully been parsed, otherwise returns 1.
     * 
     *  @param
     *  A command line string with this syntax: <cmd_name> -<arg1> <arg_values1> -<arg2> <arg_values2> ...
     * */
    int parseCommand(std::string cmd_line);

    /** Clears all available arguments into their default state. */
    void clearArgs();

  protected:
    t_cmd* parent;
    std::string cmd;
    std::string descr;

    struct arg_store_t
    {
        bool isTrue;
        char** values;
    };
    
    std::map<std::string, arg_store_t> args;

    /** This function returns true when `arg` has been specified via `execCmd` function. When argument contains additional values
     *  too, they're returned via `values`.
     */
    bool getArg(const char* arg, char** values = nullptr);
};

template<class t_cmd>
Task<t_cmd>::Task(std::string cmd, t_cmd* parent, std::string descr)
{
    this->cmd = cmd;
    this->parent = parent;
    this->descr = descr;
}

template<class t_cmd>
int Task<t_cmd>::startInteractive()
{
    return 0; // Default does nothing
}

template<class t_cmd>
int Task<t_cmd>::addArg(std::string arg)
{
    arg_store_t arg_store = {false, nullptr};
    this->args[arg] = arg_store;
    return 0;
}

template<class t_cmd>
int Task<t_cmd>::parseCommand(std::string cmd_line)
{
    std::istringstream iss(cmd);
    std::string word;
    std::string curr_arg;

    bool first = true;
    int i;
    while (iss >> word)  {
      if (first)  {
        if (this->cmd != word)  {
          return 1;
        }

        first = false;
      }
      else if (word.size() > 1 && word[0] == '-') {
          word.erase(0,1);
          if (word.size() > 1 && word[0] == '-')  {
              word.erase(0,1);
          }
          curr_arg = word;
          if (this->args.count(word))  {
            this->args.at(curr_arg).isTrue = true;
            
          }
          i = 0;
      }

      else  {
          if (this->args.count(word))  {
            this->args.at(curr_arg).values[i++] = word.data();
          }
      }
    }

    clearArgs();

    return 0;

    
}

template<class t_cmd>
bool Task<t_cmd>::getArg(const char* arg, char** values)
{
    if (this->args.count(arg))  {
        if (this->args.at(arg).values != nullptr)  {
            values = this->args.at(arg).values;
            return true;
        }
        else  {
            return this->args.at(arg).isTrue;
        }
    }
    else  {
        return false;
    }
}

template<class t_cmd>
void Task<t_cmd>::clearArgs()
{
    for (auto it = args.begin(); it != args.end(); it++)
    {
        it->second.isTrue = false;
        it->second.values = nullptr;
    }
}

#endif
