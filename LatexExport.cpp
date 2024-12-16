#include "LatexExport.h"
#include <string>
#include <iostream>

template<typename T> LatexExport::LatexExport(T items[] = nullptr)
{
	if (database)  {
		setItems(database);
	}

}

template<typename T> virtual int LatexExport::setItems(T items[])
{
	std::cerr << "Function LatexExport::setItems() not implemented!" << std::endl;
	return 1;
}

template<typename T> virtual std::string LatexExport::getLine(T item);
{
  // To be implemented 
}


template<typename T> std::string LatexExport::getTable()
{
  std::string topstr = "\\begin{table}[] \
                       \\begin{tabular}{|l|l|l|l|l|}\n"
  std::string bottomstr = "\\end{tabular} \
                           \\end{table}\n"

  std::string midstr = "";

  for (const auto& s in items)  {
    midstr += getLine(s);
  }

  return topstr + midstr + bottomstr;

}
