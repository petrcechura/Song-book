#include <string>
#include <iostream>
#include <map>

class Task
{
  public:
    Task(
        /** A command string for which this task should be called inside main loop */
        std::string cmd);
    
    /** This function contains all behaviour the task is supposed to do when called */
    virtual int Start();

    std::string getCmd() { return cmd; };


  private:
    std::string cmd;
};


class Cmd
{
  // API
  public:
    int addTask(Task task);
    int startLoop();
    int stopLoop();

  // Private properties
  private:

    const int EXIT_RET_VAL = 2;
    const char EXIT_CHAR = '-';
    
    virtual std::string getInput();
    virtual void unknownInput();

    std::map<std::string, Task> tasks;
    std::string user_input;
    bool loop_on;




    int loop();






};
