#include <string>
#include <filesystem>
#include <iostream>
#include "Database.h"
#include "lib/cmdapp/cmdapp.h"

#ifndef SONGBOOKAPP_H
#define SONGBOOKAPP_H


class SongBookApp : public Cmd
{
  using Cmd::Cmd;

  public:
    SongBookApp();

    Database* getDatabase() { return database; };

  protected:
    const int NAME_WIDTH = 30;

    /** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
    inline int countStringChars(const std::string& _str)
    {
	    std::wstring str = std::wstring_convert<std::codecvt_utf8<wchar_t>>()
					   .from_bytes(_str);
	    return str.size();
    }

    /** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
    inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30)
    {	
	    std::string str = "CANT DISPLAY (too long)" + std::string(maxWidth-23, fill);
	
	    if (_str.size() < maxWidth)  {
		    str = _str + std::string(NAME_WIDTH - countStringChars(_str), fill);
	    }

	    return str;
    }

    //====================
    // DATABASE MANAGEMENT
    //====================
    /** TODO descr */
    Database* database;

};


#endif
