#include <string>
#include <iostream>
#include <iomanip>
#include "BackupTask.h"
#include "SongBookApp.h"

int BackupTask::executeCommand()
{

  int i = parent->getDatabase()->makeBackup();
  if (!i)  {
    parent->printInteractive("Backup file has been created...", 2);
  }
  else  {
    parent->printInteractive("Error while creating backup file ...", 2);
  }

  return 0;
}
