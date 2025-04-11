#include <string>
#include <iostream>
#include <iomanip>
#include "ExitTask.h"
#include "SongBookApp.h"

int ExitTask::Start()
{
  parent->stopLoop();
  return 0;
}
