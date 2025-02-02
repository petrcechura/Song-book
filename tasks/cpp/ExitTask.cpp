#include <string>
#include <iostream>
#include <iomanip>
#include "../h/ExitTask.h"
#include "../../SongBookApp.h"

int ExitTask::Start()
{
  parent->stopLoop();
  return 0;
}
