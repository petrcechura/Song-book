#include "WindowServer.h"
#include "WTask.h"
#include "Window.h"
#include <functional>
#include <iostream>

class PrintTask : public WTask<WindowServer>
{
public:
    PrintTask(std::string name, WindowServer* parent, std::string description) 
      : WTask<WindowServer>(name, parent, description) {};

    virtual int Execute(char command) override {
      switch(command)  {
        case 'p': doPrint();
                  break;
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

protected:
    int cnt = 0;
};



int main(int argc, char* argsv[])
{
  WindowServer* window_server = new WindowServer();

  Window* w = new Window("Main Screen", 40, 15, 10, 10);
  PrintTask* print = new PrintTask("Print", window_server, "descr");
  print->AddWindow(w);
  window_server->AddWindow(w);
  window_server->AddTask(print);

  window_server->Init();
  window_server->Loop();
  delete window_server;

  return 0;
}
 