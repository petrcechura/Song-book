#include <string>
#include <iostream>
#include <iomanip>
#include "ExitTask.h"
#include "SongBookApp.h"

int ExitTask::executeCommand()
{
  parent->stopLoop();
  return 0;
}
