#include <string>
#include <filesystem>
#include <iostream>
#include "SongDatabase.h"
#include "cmdapputils.h"
#include "json.hpp"

#ifndef SONGBOOKAPP_H
#define SONGBOOKAPP_H

#define TITLE_WIDTH 30
#define ARTIST_WIDTH 30

/** A specific implementation of `CommandServer` class, `SongBookApp` the main class that provides
 *  API to start/stop Command-line application where tasks can be called, thus
 *  allowing to indirectly work with a *songbook* database.
 *
 *  New tasks (`Task`) can be simply added via specific methods and their behaviour is encapsulated
 *  so they can be managed/deleted/added independetly. */
class SongBookApp : public CommandServer
{
  using CommandServer::CommandServer;

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
    SongDatabase* getDatabase() { return database; };

    static void printSong(const std::string& id, const std::string& name, const std::string& author);
    static void printSongListHeader();
    static void printSongListBottom();

    static void printInteractive(const std::string& text, unsigned int indentation=0, bool newline=true);

    /** This function returns number of characters inside string variable, regardless of character format (UNICODE/ASCII) */
    static inline int countStringChars(const std::string& _str);

    /** This function returns an aligned string with set width, regardless of characters format (UNICODE/ASCII) */
    static inline std::string alignString(const std::string& _str, char fill = ' ', int maxWidth = 30);

    /** This function executes serie of commands separated by ';' character. */
    void executeCommands(std::string cmd_line, bool exitWhenDone=false);

  protected:
    /** A `startHook()` implementation */
    void startHook() override;
    /** A `stopHook()` implementation */
    void stopHook() override;

    //====================
    // DATABASE MANAGEMENT
    //====================
    /** songbook database instance */ 
    SongDatabase* database;

    std::string order = "ID";

};


#endif
