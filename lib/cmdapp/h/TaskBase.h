#include <string>

#ifndef TASKBASE_H
#define TASKBASE_H


/** TODO descr. */
class TaskBase {
  public:
    virtual int Start() = 0;
    virtual ~TaskBase() = default;
    virtual std::string getCmd() = 0;
    virtual std::string getDescr() = 0;
};

#endif 
