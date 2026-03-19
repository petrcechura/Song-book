#ifndef SONGBOOKAPP_H
#define SONGBOOKAPP_H

#include <string>
#include <filesystem>
#include <iostream>
#include "SongDatabase.h"
#include "SongBookFormatter.h"
#include "WindowServer.h"
#include "json.hpp"

/** A specific implementation of `CommandServer` class, `SongBookApp` the main class that provides
 *  API to start/stop Command-line application where tasks can be called, thus
 *  allowing to indirectly work with a *songbook* database.
 *
 *  New tasks (`Task`) can be simply added via specific methods and their behaviour is encapsulated
 *  so they can be managed/deleted/added independetly. */
class SongBookApp : public WindowServer
{
  using WindowServer::WindowServer;

  public:
    /** A default contructor that creates a songbook database instance
     *  and adds certains tasks to a *task container* */
    SongBookApp();
    virtual ~SongBookApp() = default;

    enum class app_state_t {
      SONG_BROWSE,
      COLLECTION_BROWSE,
      ADD_TO_COLLECTION
    };
    
    /** Returns an instance of a database, allowing tasks to operate with that.
     *
     *  TODO: This is wrong, database shall **NOT** be accessable via API.
     *        Contemplate a way how to allow only tasks to operate with database
     *        (and maybe only a certain ones)*/
    SongDatabase* getDatabase() { return database; };
    app_state_t getState() { return state; };
    void setState(app_state_t state) { this->state = state; };

    /** This function executes serie of commands separated by ';' character. */
    void executeCommands(std::string cmd_line, bool exitWhenDone=false);

    std::string SongEditor(std::string lyrics);

  protected:
    /** A `startHook()` implementation */
    virtual void StartHook() override;
    /** A `stopHook()` implementation */
    virtual void StopHook() override;

    virtual void afterExecuteHook() override;

    //====================
    // DATABASE MANAGEMENT
    //====================
    /** songbook database instance */ 
    SongDatabase* database;

    std::string order = "ID";
    app_state_t state = app_state_t::SONG_BROWSE;
    std::map<std::string, std::string> task_legend;
};

#endif
