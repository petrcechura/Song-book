#include <string>
#include <iostream>
#include <iomanip>
#include "ExitTask.h"
#include "SongBookApp.h"

int ExitTask::executeCommand(int error_code)
{
  parent->stopLoop();
  return 0;
}
