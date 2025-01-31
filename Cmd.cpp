#include <string>
#include <iostream>
#include "Cmd.h"



Task::Task(std::string cmd)
{
  this->cmd = cmd;
}

int Task::Start()
{
  // TODO implemented by end-user
  
  return 0;
}


int Cmd::addTask(Task* task)
{
  tasks[task->getCmd()] = task;

  return 0;
}

std::string getInput()
{
  std::string user_input;

  std::cout << ">> ";
	std::getline(std::cin, user_input);

  return user_input;
}

void Cmd::unknownInput()
{
  std::cout << "Unknown input '" << user_input << "'!" << std::endl;
}

int Cmd::loop()
{
  while(loop_on)  {
    user_input = getInput();

    if (user_input == std::to_string(EXIT_CHAR))  {
      stopLoop();
    }

    else if (user_input == "")  {
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

int Cmd::startLoop()
{
  loop_on = true;
  
  return loop();
}

int Cmd::stopLoop()
{
  loop_on = false;

  return 0;
}
