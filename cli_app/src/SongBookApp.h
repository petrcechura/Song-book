#include <string>
#include <filesystem>
#include <iostream>
#include "Database.h"
#include "cmdapputils.h"
#include "json.hpp"

#ifndef SONGBOOKAPP_H
#define SONGBOOKAPP_H

/** A specific implementation of `CmdApp` class, `SongBookApp` the main class that provides
 *  API to start/stop Command-line application where tasks can be called, thus
 *  allowing to indirectly work with a *songbook* database.
 *
 *  New tasks (`Task`) can be simply added via specific methods and their behaviour is encapsulated
 *  so they can be managed/deleted/added independetly. */
class SongBookApp : public Cmd
{
  using Cmd::Cmd;

  friend class HelpTask;

  public:
    /** A default contructor that creates a songbook database instance
     *  and adds certains tasks to a *task container* */
    SongBookApp();
    
    /** Returns an instance of a database, allowing tasks to operate with that.
     *
     *  TODO: This is wrong, database shall **NOT** be accessable via API.
     *        Contemplate a way how to allow only tasks to operate with database
     *        (and maybe only a certain ones)*/
    Database* getDatabase() { return database; };



  protected:
    const int NAME_WIDTH = 30;

    /** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
    inline int countStringChars(const std::string& _str);

    /** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
    inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30);
    
    /** A `startHook()` implementation */
    void startHook() override;
    /** A `stopHook()` implementation */
    void stopHook() override;

    //====================
    // DATABASE MANAGEMENT
    //====================
    /** songbook database instance */ 
    Database* database;

};


#endif
