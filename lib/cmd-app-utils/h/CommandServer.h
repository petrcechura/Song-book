#include <string>
#include <iostream>
#include "json.hpp"
#include <map>
#include "TaskBase.h"

#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

/** This is a top-level class that is supposed to be initialized,
 *  and serves as:
 *    - container for all the Tasks
 *    - proxy to all *settings* (exit chars, constants) and aux. classes
 *    - main API for managing the **main loop** where Command-line app is running
 *
 *    By inheriting this class, user may add his/her own properties, define *hooks*
 *    or bond own classes into the application hiearchy to fully customize
 *    his/her implementation. 
 */
class CommandServer
{

  private:
    enum class ErrorCode {
      SUCCESS
    };

  // API
  public:
    /** Adds a `TaskBase` object into the *task container*, thus allow its 
     *  implementation to be part of Command-line application. 
     *
     *  Note that since `TaskBase` is an abstract class, its expected that 
     *  the `task` object will be `TaskBase` derivative, actually */
    int addTask(TaskBase* task);

    /** This method simply starts the **main loop**.
     *
     *  The **main loop** is a core of Command-line application, allowing user_input
     *  to call individual tasks (added to the `CmdApp` via their `cmd` property).
     *
     *  Before the loop start, `startHook()` method is called (if overriden) 
     */
    int startLoop();

    /** Stops the **main loop**, hence yields the control over the cmd-line.
     *
     *  Between stopping the **main loop** and yielding control, `stopHook()` method
     *  is called (if overriden)
     */
    int stopLoop();
    
    // TODO implement
    CommandServer(nlohmann::json _config);
    CommandServer();
    ~CommandServer();
    
    /** Returns an EXIT_CHAR property, that user may use to terminate the **main loop**
     *  when using it. 
     */
    char getExitChar() { return EXIT_CHAR; };

    /**  Executes a command string, followed by additional arguments 
     *   Example: <Task Name> -<arg1> -<arg2>
    */
    int executeTaskString(std::string task_string);

    std::string getInput(unsigned int indentation = 0);

  // Private properties
  protected:

    const int EXIT_RET_VAL = 2;
    char EXIT_CHAR = '-';
    
    void unknownInput();

    std::map<std::string, TaskBase*> tasks;
    std::string user_input;
    bool loop_on;

    int loop();

    virtual void startHook() {};
    virtual void stopHook() {};

};

#endif
