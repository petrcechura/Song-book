#ifndef LATEX_EXPORT_H
#define LATEX_EXPORT_H

#include <iostream>
#include <string>
#include "json/json.hpp"


/** This is a template class for generating a Latex table file with specified <T> object as a data inside. 
 * This class is considered to be virtual, thus user is forced to derive and write his/her own implementation
 * with a <T> object and custom table formatting as desired 
*/
template <typename T> class LatexExport
{
    using json_t = nlohmann::json;

public:

	LatexExport(T items[]);
	~LatexExport();

	/** Store an array of <T> data objects inside a class for further processing.
	 *  Returns 0 is data is valid, 1 otherwise.	
	 * 
	 *  This method is considered to be overriden by end-user to properly assign <T> properties
	 *	to headers defined inside *json file*
	 **/
	virtual int setItems(T items[]);

	/** Return a LaTeX line of a <T> instance
	 *
	 *  Simple implementation of this function is provided by default,
	 *  hovewer user may override and customize its format  
	 */
	virtual std::string getLine(T item);
	
	/** Return a LaTeX table in raw string of entire database (including headers) */
	std::string getTable();
	
	/** Load header strings from a json file specified in an argument
	 *	Header file should contain only names for individual table headers,
	 *  as follows: {"header1": "", "header2": "", ...}
	 *  
	 * 	Function returns 0 if parse was successful, 1 otherwise
	 */
	int void loadHeaders(std::string json_file);

private:
	/** Filepath of an output LaTeX file */
	std::string output_latex_file;

	/** A json container that contains custom headers to be displayed in LaTeX generated pdf file */
	json_t headers;

}

#endif
