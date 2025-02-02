#include <string>
#include <iostream>
#include <iomanip>
#include "../h/BackupTask.h"
#include "../../SongBookApp.h"

int BackupTask::Start()
{
  parent->getDatabase()->saveJsonFile();

  int i = parent->getDatabase()->makeBackup();
  if (!i)  {
    std::cout << "backup file has been created..." << std::endl;
  }
  else  {
    std::cout << "error while creating backup file..." << std::endl;
  }

  return 0;
}
