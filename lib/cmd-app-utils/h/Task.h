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
        std::string task_name,
        /** A reference to a parent class */
        t_cmd* parent,
        /** A description to be printed when calling `help` */
        std::string description="no description provided");
    
    /** 
     *  Starts the interactive (CLI) approach to obtain all required arguments for task execution.
     */
    int startInteractive() override;

    /** 
     *  Executes this task. For execution, arguments obtained via `Start` (interactive) or `parseCommand` (non-interactive) are used.
     */
    int executeCommand() override;

    void endInteractive(int error_code) override;
    
    /** An aux. setter for a description */
    void setDescription(std::string d) { this->description = d; };
    
    /** A getter for `cmd` string */
    std::string getCommandString() override { return task_name; };
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

    static std::string values2string(arg_store_t arg);

  protected:
    t_cmd* parent;
    std::string task_name;
    std::string description;
    
    std::map<std::string, arg_store_t> arguments;

    /** This function returns arg_store_t object of specified argument. If the argument does not exist, null is returned
     */
    arg_store_t getArgument(const char* arg);
};

template<class t_cmd>
Task<t_cmd>::Task(std::string cmd, t_cmd* parent, std::string descr)
{
    this->task_name = cmd;
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

    int word_cnt = 0;
    std::string word;
    std::string prev_arg;
    for (int i = 0; i < cmd_line.size(); i++)  {
        if (cmd_line[i] == ' ' || i == cmd_line.size()-1)  {
            // when on end of the string, process last character too
            if (i==cmd_line.size()-1 && cmd_line[i] != ' ')
                word += cmd_line[i];

            // check if cmd_line has valid `task_name`
            if (word_cnt == 0)  {
                if (word != this->task_name)  {
                    return 1;
                }
            }

            // previous word was `-argument`
            else if (word[0] == '-')  {
                for (const auto s : arguments)  {
                }
                if (this->argumentExists(word.c_str()))  {
                    arg_store_t a = {true,{}};
                    this->updateArgument(word, a, false);
                    prev_arg = word;
                }
                else  {
                    std::cout << "Argument " << word << " does not exist for " << this->task_name << "!" << std::endl;
                    return 2;
                }                
                
            }
            // previous word was `value`
            else {
                if (!prev_arg.empty() && this->argumentExists(prev_arg.c_str()))  {
                    arg_store_t a = this->getArgument(prev_arg.c_str());
                    a.values.push_back(word);
                    this->updateArgument(prev_arg, a);
                }
            }
            word.clear();
            word_cnt++;
        }
        else {
            word += cmd_line[i];
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
        it->second.values = {};
    }
}

template<class t_cmd>
std::string Task<t_cmd>::values2string(arg_store_t o)
{
    std::string s = "";
    bool first = true;
    for (const auto& c : o.values)  {
        if (first)  {
            s = c;
            first = false;
        }
        else  {
            s += " " + c;
        }
    }
    return s;
}



#endif
