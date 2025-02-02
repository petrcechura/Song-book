#include <string>
#include <iostream>
#include <map>
#include "TaskBase.h"

#ifndef CMD_H
#define CMD_H

/** TODO descr. */
class Cmd
{

  // API
  public:
    int addTask(TaskBase* task);
    int startLoop();
    int stopLoop();
    
    // TODO implement
    Cmd();
    ~Cmd();

    char getExitChar() { return EXIT_CHAR; };

  // Private properties
  protected:

    const int EXIT_RET_VAL = 2;
    const char EXIT_CHAR = '-';
    
    std::string getInput();
    void unknownInput();

    std::map<std::string, TaskBase*> tasks;
    std::string user_input;
    bool loop_on;

    int loop();

    virtual void startHook() {};
    virtual void stopHook() {};

};

#endif
