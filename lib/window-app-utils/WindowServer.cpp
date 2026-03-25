#include <ncurses.h>
#include <string>
#include "WindowServer.h"
#include "TaskBase.h"
#include "Window.h"
#include <locale.h>
#include <iostream>


WindowServer::~WindowServer()
{
    for (auto const& [name, window] : windows)  {
        window->Delete();
        delete window; 
    }

    for (auto const& [name, task] : tasks)  {
        delete task; 
    }
}

void WindowServer::Refresh()
{
    for (auto const& [name, window] : windows)  {
        window->Refresh();
    }
}


WindowServer::WindowServer() 
{

}

void WindowServer::Init()
{   
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();

    for (auto const& [name, window] : windows)  {
        window->Init();
        refresh();
        window->Refresh(); 
    }
}

void WindowServer::Loop()
{
    int c = 'x';

    StartHook();

    while(c != this->EXIT_CHAR) {
        c = getch();

        for (auto const& [name, task] : tasks)  {
            task->Execute(c);
        }
        
        afterExecuteHook();
    }

    StopHook();
}

void WindowServer::AddWindow(Window* window)
{
    this->windows[window->GetName()] = window;
}


int WindowServer::AddTask(TaskBase* task)
{
    this->tasks[task->GetName()] = task;

    return 0;
}

int WindowServer::executeTaskString(std::string task_string)
{
    // TODO !!!
    /*
  std::string task_name = task_string.substr(0, task_string.find(" "));
    
  const auto it = tasks.find(task_name);
  if (it != tasks.end())  {
    if (!it->second->parseCommand(task_string))  {
      return it->second->executeCommand(0);
    }
    else  {
      std::cout << "Could not parse task_string '" << task_string << "'" << std::endl;
      return 2;
    }
      
  }
  else {
    std::cout << "Task " << task_name << " not found!" << std::endl;
    return 1;
  }
    */
  return 0;
}

void WindowServer::StartHook()
{

}

void WindowServer::StopHook()
{

}

void WindowServer::afterExecuteHook()
{

}