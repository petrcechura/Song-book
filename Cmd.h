#include <string>
#include <iostream>
#include <map>

// forward decl.
class Cmd;

class Task
{

  public:
    Task(
        /** A command string for which this task should be called inside main loop */
        std::string cmd,
        /** A reference to a parent class */
        // TODO replace with inner class inside `Cmd`, to obtain access only to database
        Cmd* parent);
    
    /** This function contains all behaviour the task is supposed to do when called */
    virtual int Start();

    void setDescr(std::string d) { this->descr = d; };

    std::string getCmd() { return cmd; };
    std::string getDescr() { return descr; };

  private:

    Cmd* parent;
    std::string cmd;
    std::string descr;
};


class Cmd
{

  // API
  public:
    int addTask(Task* task);
    int startLoop();
    int stopLoop();
    
    // TODO implement
    Cmd();
    ~Cmd();

    char getExitChar() { return EXIT_CHAR; };

  // Private properties
  private:

    const int EXIT_RET_VAL = 2;
    const char EXIT_CHAR = '-';
    
    std::string getInput();
    void unknownInput();

    std::map<std::string, Task*> tasks;
    std::string user_input;
    bool loop_on;

    int loop();

};
