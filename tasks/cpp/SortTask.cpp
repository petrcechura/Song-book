#include <string>
#include <iostream>
#include <iomanip>
#include "SortTask.h"
#include "SongBookApp.h"


int SortTask::executeCommand(int error_code)
{
  if (error_code == SUCCESS)  {
    if (argumentExists("-criteria", true))  {
      std::string criteria = getArgument("-criteria").values[0];

      int error = parent->getDatabase()->changeOrder(criteria);

      if (error == 0) {
        return SUCCESS;
      }
      else {
        return CRITERIA_NOT_FOUND;
      }
    }
    else  {
      return ARG_EMPTY;
    }
  }
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

  return SUCCESS;
}

void SortTask::endInteractive(int error_code)
{
  switch(error_code)
  {
      case SUCCESS: 
        parent->printInteractive("Sort has been made succesfully!", 1); break;
      case OK_EXIT_CHAR:
        break;
      case CRITERIA_NOT_FOUND: 
        parent->printInteractive("This sorting criteria does not exist!", 1); break;
      case ARG_EMPTY: 
        parent->printInteractive("Argument -criteria is empty!", 1); break;
      default:
        parent->printInteractive("Could not sort a database due to unknown error", 1);

  }
}