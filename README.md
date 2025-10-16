# Command-line Songbook Application
An SQL-based song database CLI application, able to:
  * CRUD operations over songs in database (title, artist)
  * Generate a LaTeX Songbook from database
  * Find chords and lyrics on Web automatically
  * Run app either in interactive or batch mode

# Installation setup
## Dependencies
  * g++ (20+)
  * curl (8.16)
  * make (4.3)
  * pdflatex
  * pandoc
  * rust
## How to build an app
### Linux
```
git clone git@github.com:petrcechura/Song-book.git
make build
```
### Windows
**Currently unavailable**

# How to use an app
## Configuration file
Many of the app properties are managed via `CONFIG_FILE`, which is structure `.json` file. See `default_config.json` file to see available properties.

## Batch mode
By default, application is run in *batch mode*, hence reads command from `-cmd` argument and exits. Commands are separated with ';' character. Alternatively, these commands may be specified in `CONFIG_FILE` under `commons/cmd` property.
See section `Tasks` for available commands.
Usage:
```
CONFIG_FILE=<path-to-json-file> \
COMMANDS=<<cmd1>;<cmd2>;<cmd3>...> \
make run
```
## Interactive mode
When `-loop` argument is used, app is executed in an interactive CLI mode.
```
CONFIG_FILE=<path-to-json-file> \
COMMANDS=<<cmd1>;<cmd2>;<cmd3>...> \
LOOP=true \
make run
```
To exit this interactive mode, type '-' (or different character specified in `CONFIG_FILE` under `commons/exit_char`).

## Tasks
*TODO Add proper description of all tasks available (interactive and batch mode)*

# TODO
  - [ ] Refactor code into Cmake project management (instead of Makefile)
  - [ ] Comment out all functions and task
  - [ ] Allow arguments in interactive mode too
  - [x] Add proper error handling to all tasks
  - [x] Add Bard converter support
  - [ ] Remove unused libraries and for others, make them git submodules to provide reference
  - [ ] Make `GatherTask` thread-based, so it's killable
  - [ ] Assign std::err to a file, so debug is possible the easy way
  - [ ] Untie `bard` tool location from the app properly
  - [x] Make a translator layer in SongDatabase between ID and no of song in `ListTask`