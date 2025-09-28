#include <string>
#include <iostream>
#include <iomanip>
#include "SortTask.h"
#include "SongBookApp.h"


int SortTask::executeCommand(int error_code)
{
  if (argumentExists("-criteria", true))  {
    std::string criteria = getArgument("-criteria").values[0];

    return parent->getDatabase()->changeOrder(criteria);
  }
  else  {
    return 2;
  }
  return 0;
}

int SortTask::startInteractive()
{
  std::string criteria;

  parent->printInteractive("Type a sorting criteria");
  criteria = parent->getInput(1);

  if (criteria != "")  {
    std::vector<std::string> s = {criteria};
    updateArgument("-criteria", {false, s});
  }

  return 0;
}

void SortTask::endInteractive(int error_code)
{
  if (!error_code)  {
    parent->printInteractive("Sort has been made succesfully!", 1);
  }
  else if (error_code == 1)  {
    parent->printInteractive("This sorting criteria does not exist!", 1);
  }
  else if (error_code == 2)  {
    parent->printInteractive("Argument -criteria is empty!", 1);
  }
}