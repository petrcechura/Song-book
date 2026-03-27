#ifndef PRINTTASK_H
#define PRINTTASK_H

#include "WindowServer.h"
#include "Window.h"
#include "WTask.h"
#include <string>

class PrintTask : public WTask<WindowServer>
{
public:
    PrintTask(std::string name, WindowServer* parent, std::string description) 
      : WTask<WindowServer>(name, parent, description) {};

    virtual int Execute(char command) override {
      switch(command)  {
        case 'p': doPrint();
                  break;
        case 'l': doPrintLog();
        default: return 1;
      }

      return 0;
    };
    virtual int ParseCommand(std::string cmd_line) override {
      return 0;
    };
    void doPrint() {
      if (this->windows.count("Main Screen") > 0)  {
        std::string s = "ssssssqqdsddsfdfssssssssssssssssssssssssssssssssssssssdfdgfdfdgfdgfd cnt: " + std::to_string(cnt);
        windows["Main Screen"]->Print(s);
        windows["Main Screen"]->Refresh();
        cnt++;
      }
    };

    void doPrintLog()  {
      if (this->windows.count("Log Screen") > 0)  {
        std::string s = "Written " + std::to_string(cnt);
        windows["Log Screen"]->Clear();
        windows["Log Screen"]->Print(s);
        windows["Log Screen"]->Refresh();
      }
    }

protected:
    int cnt = 0;
};


#endif