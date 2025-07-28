#include <string>
#include <iostream>
#include <sstream>
#include "../h/Cmd.h"
#include "../h/TaskBase.h"

int Cmd::addTask(TaskBase* task)
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

    else if (user_input.empty())  {
      continue;
    }
    else  {
      if (auto it = tasks.find(user_input); it != tasks.end())  {
        it->second->startInteractive();
        it->second->executeCommand();
        it->second->
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
  startHook();

  loop_on = true;
  
  return loop();
}

int Cmd::stopLoop()
{
  loop_on = false;

  stopHook();

  return 0;
}


int Cmd::execCmd(std::string cmd)
{

  std::istringstream iss(cmd);
  std::string word;
  while (iss >> word)  {
    
    const auto it = tasks.find(word);
    if (it != tasks.end())  {
      it->second->execCmd(cmd);
      return 0;
    }
    else {
      return 1;
    }
  }

  return 1;

}
