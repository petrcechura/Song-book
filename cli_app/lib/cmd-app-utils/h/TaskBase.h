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
    virtual int Start(bool interactive = true) = 0;
    virtual ~TaskBase() = default;
    virtual std::string getCmd() = 0;
    virtual std::string getDescr() = 0;
};

#endif 
