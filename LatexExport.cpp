#include "LatexExport.h"
#include <string>
#include <iostream>

template<typename T> LatexExport::LatexExport(T items[] = nullptr)
{
	if (database)  {
		setItems(database);
	}

}

template<typename T> int LatexExport::setItems(T items[])
{
	std::cerr << "Function LatexExport::setItems() not implemented!" << std::endl;
	return 1;
}

template<typename T> virtual std::string LatexExport::getLine(T item);
{
	

}


std::string LatexExport::getTable()
{


}
