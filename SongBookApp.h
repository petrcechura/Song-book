#include "Cmd.h"
#include <string>
#include <iostream>


/** Adds a song to a database */
class Add : public Task
{
  virtual int Start();
};

class Modify : public Task
{
  virtual int Start();
};

class Remove : public Task
{
  virtual int Start();
};


class SongBookApp : Cmd
{

  public:
    SongBookApp();

  private:
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
  
};





