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

  friend class HelpTask;

  public:
    SongBookApp();

    Database* getDatabase() { return database; };



  protected:
    const int NAME_WIDTH = 30;

    /** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
    inline int countStringChars(const std::string& _str);

    /** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
    inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30);

    void startHook() override;
    void stopHook() override;

    //====================
    // DATABASE MANAGEMENT
    //====================
    /** TODO descr */
    Database* database;

};


#endif
