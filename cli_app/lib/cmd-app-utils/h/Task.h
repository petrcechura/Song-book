#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "TaskBase.h"
#include <sstream>

#ifndef TASK_H
#define TASK_H

struct arg_store_t
{
    bool isTrue;
    std::vector<std::string> values;
};

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
        std::string command_string,
        /** A reference to a parent class */
        t_cmd* parent,
        /** A description to be printed when calling `help` */
        std::string description="no description provided");
    
    /** 
     *  Starts the interactive (CLI) approach to obtain all required arguments for task execution.
     */
    int startInteractive() override;

    void endInteractive(int error_code) override;
    
    /** An aux. setter for a description */
    void setDescription(std::string d) { this->description = d; };
    
    /** A getter for `cmd` string */
    std::string getCommandString() override { return command_string; };
    /** A getter for a description */
    std::string getDescription() override { return description; };
    
    /** Assigns argument for a task to be used after the command name to specify additional actions
     *  @param arg
     *  Specifies the argument name. Use "-<arg>" syntax.
     *  @param arg_contents
     *  (optional) Assign values to the argument. If not specified, default {false, null} are used.
     *  @param forceCreate
     *  If argument with specified name does now exist, it's created unless this option is false (default true).
    */
    int updateArgument(std::string arg, arg_store_t arg_contents={false, {}}, bool forceCreate=true);
    
    /** 
     *  Executes this task. For execution, arguments obtained via `Start` (interactive) or `parseCommand` (non-interactive) are used.
     */
    int executeCommand() override;
    
    /** Attempts to parse a command line string (arguments separation) and stores its contents into this class.
     * 
     *  Returns 0 when cmd string has successfully been parsed, otherwise returns 1.
     * 
     *  @param
     *  A command line string with this syntax: <cmd_name> -<arg1> <arg_values1> -<arg2> <arg_values2> ...
     * */
    int parseCommand(std::string cmd_line) override;

    /** Clears all available arguments into their default state. */
    void clearArguments();

    bool argumentExists(const char* arg, bool checkIfValExists=false);

  protected:
    t_cmd* parent;
    std::string command_string;
    std::string description;
    
    std::map<std::string, arg_store_t> arguments;

    /** This function returns arg_store_t object of specified argument. If the argument does not exist, null is returned
     */
    arg_store_t getArgument(const char* arg);
};

template<class t_cmd>
Task<t_cmd>::Task(std::string cmd, t_cmd* parent, std::string descr)
{
    this->command_string = cmd;
    this->parent = parent;
    this->description = descr;
}

template<class t_cmd>
int Task<t_cmd>::startInteractive()
{
    return 0; // Default does nothing
}

template<class t_cmd>
void Task<t_cmd>::endInteractive(int error_code)
{
    // Default does nothing
}

template<class t_cmd>
int Task<t_cmd>::updateArgument(std::string arg, arg_store_t arg_contents, bool forceCreate)
{
    if (this->arguments.count(arg))  {
        this->arguments.at(arg) = arg_contents;
        return 0;
    } 
    else if (forceCreate)  {
        this->arguments.insert({arg, arg_contents});
        return 0;
    }
    else  {
        return 1;
    }
    return 0;
}



template<class t_cmd>
int Task<t_cmd>::parseCommand(std::string cmd_line)
{
    std::istringstream iss(command_string);
    std::string word;
    std::string curr_arg;

    bool first = true;
    int i;
    while (iss >> word)  {
      if (first)  {
        if (this->command_string != word)  {
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
          if (this->arguments.count(word))  {
            this->arguments.at(curr_arg).isTrue = true;
            
          }
          i = 0;
      }

      else  {
          if (this->arguments.count(word))  {
            this->arguments.at(curr_arg).values[i++] = word.data();
          }
      }
    }

    return 0;
}

template<class t_cmd>
int Task<t_cmd>::executeCommand()
{
    return 0;
}

template<class t_cmd>
arg_store_t Task<t_cmd>::getArgument(const char* arg)
{
    if (this->arguments.count(arg))  {
        return this->arguments.at(arg);
    }
    else  {
        return {};
    }
}

template<class t_cmd>
bool Task<t_cmd>::argumentExists(const char* arg, bool checkIfValExists)
{   
    if (this->arguments.count(arg))  {
        if (checkIfValExists)  {
            return (this->arguments.at(arg).values.size() > 0);
        }
        else  {
            return true;
        }
    }
    else  {
        return false;
    }
}

template<class t_cmd>
void Task<t_cmd>::clearArguments()
{
    for (auto it = arguments.begin(); it != arguments.end(); it++)
    {
        it->second.isTrue = false;
        it->second.values = nullptr;
    }
}

#endif
