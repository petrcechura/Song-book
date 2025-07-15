#include <string>
#include <iostream>
#include <iomanip>
#include "BackupTask.h"
#include "SongBookApp.h"

int BackupTask::Start()
{

  int i = parent->getDatabase()->makeBackup();
  if (!i)  {
    std::cout << "backup file has been created..." << std::endl;
  }
  else  {
    std::cout << "error while creating backup file..." << std::endl;
  }

  return 0;
}
