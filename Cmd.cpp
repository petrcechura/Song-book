#include <string>
#include <iostream>
#include "Cmd.h"



Task::Task(std::string cmd, Cmd* parent)
{
  this->cmd = cmd;
  this->parent = parent;
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

std::string Cmd::getInput()
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

Cmd::Cmd()
{

}

Cmd::~Cmd()
{

}

int Cmd::loop()
{
  while(loop_on)  {
    user_input = getInput();

    if (user_input == std::string(1, EXIT_CHAR))  {
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
