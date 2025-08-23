#include <string>
#include <iostream>
#include <sstream>
#include "../h/CommandServer.h"
#include "../h/TaskBase.h"

int CommandServer::addTask(TaskBase* task)
{
  tasks[task->getCommandString()] = task;

  return 0;
}

std::string CommandServer::getInput(unsigned int indentation)
{
  std::string user_input;

  std::cout << std::string(indentation*2, ' ') + ">> ";
	std::getline(std::cin, user_input);

  return user_input;
}

void CommandServer::unknownInput()
{
  std::cout << "Unknown input '" << user_input << "'!" << std::endl;
}

CommandServer::CommandServer()
{

}

CommandServer::~CommandServer()
{

}

int CommandServer::loop()
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
        it->second->endInteractive(it->second->executeCommand());
      }
      else  {
        unknownInput();
      }
    }
  }

  return 0;
}

int CommandServer::startLoop()
{
  startHook();

  loop_on = true;
  
  return loop();
}

int CommandServer::stopLoop()
{
  loop_on = false;

  stopHook();

  return 0;
}


int CommandServer::executeTaskString(std::string task_string)
{

  std::string task_name = task_string.substr(0, task_string.find(" "));
    
  const auto it = tasks.find(task_name);
  if (it != tasks.end())  {
    if (!it->second->parseCommand(task_string))  {
      int err = it->second->executeCommand();
      return err;
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
  return 0;
}
