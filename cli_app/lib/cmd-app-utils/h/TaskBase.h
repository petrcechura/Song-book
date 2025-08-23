#include <string>

#ifndef TASKBASE_H
#define TASKBASE_H


/** An abstract base class that contains all the default behaviour tasks are supposed to have
 *  in all implementations.
 *
 *  `TaskBase` is immediately inherited by `Task` class, which is fairly similar class expect
 *  for the fact its a *template class*. The intent of having these two similar classes is to
 *  differ *template* and *non-template* methods, which greetly simplifes further management.
 */
class TaskBase {
  public:
    virtual int startInteractive() = 0;
    virtual void endInteractive(int error_code) = 0;
    virtual ~TaskBase() = default;
    virtual std::string getCommandString() = 0;
    virtual std::string getDescription() = 0;
    virtual int parseCommand(std::string cmd_line = "") = 0;
    virtual int executeCommand() = 0;
};

#endif 
