#include <string>
#include <iostream>
#include "../h/CmdApp.h"
#include "../h/TaskBase.h"

int CmdApp::addTask(TaskBase* task)
{
  tasks[task->getCmd()] = task;

  return 0;
}

std::string CmdApp::getInput()
{
  std::string user_input;

  std::cout << ">> ";
	std::getline(std::cin, user_input);

  return user_input;
}

void CmdApp::unknownInput()
{
  std::cout << "Unknown input '" << user_input << "'!" << std::endl;
}

CmdApp::CmdApp()
{

}

CmdApp::~CmdApp()
{

}

int CmdApp::loop()
{
  while(loop_on)  {
    user_input = getInput();

    if (user_input == std::string(1, EXIT_CHAR))  {
      stopLoop();
    }

    else if (user_input.empty())  {
      continue;
    }
    else  {
      if (auto it = tasks.find(user_input); it != tasks.end())  {
        it->second->Start();
      }
      else  {
        unknownInput();
      }
    }
  }

  return 0;
}

int CmdApp::startLoop()
{
  startHook();

  loop_on = true;
  
  return loop();
}

int CmdApp::stopLoop()
{
  loop_on = false;

  stopHook();

  return 0;
}
