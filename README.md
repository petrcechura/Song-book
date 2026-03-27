> [!WARNING]
> This repository is still under development and its binary may not be stable due to lack of proper testing!

# Command-line Songbook Application
A simple SQL-based song database CLI application for song lyrics/chords management.

Supported actions are:
  * CRUD operations over songs in database (title, artist)
  * Generate a PDF Songbook booklet using external `bard` tool
  * Scrap chords and lyrics from specific websites automatically
  * Create collections for your songs for easy management

# Installation
## Install dependencies
> [!NOTE]
> For czech sort, please install locale cs_CZ.UTF-8 too (otherwise special characters are not handled properly).

### Arch Linux

```
sudo pacman -S libxml2 curl rust ncurses g++ make 
```

Initialize `bard` tool as a PDF generator:

> [!NOTE]
> Song-book uses `bard` tool to create final PDF file.
> See original [bard repository](https://github.com/vojtechkral/bard) for more details about the project.

```
cd lib/bard && cargo install -f bard && make release
```

Build your application:
```
cmake build . && \
cmake --build .
```

Run application (full-screen is recommended):
```
./app -config <your-config-file>
```

### Windows
Please follow [this tutorial](https://wiki.archlinux.org/title/Installation_guide).

# How to use an app

Song-book app is *command-line only* and shall be used with full-screen mode of a terminal window. Controls are simple and shall be shown at every moment
in the window (*commands window*) on the right side.

To exit an application, enter *ESC* key.

## Configuration file

A single JSON configuration file is used to configure built app and modify its behaviour after compiling. Usage of such config file is **highly recommended**, as some of the features are not available without it.

Configuration file has structure as follows:
```
{
  "commons" : {
    "text_editor" : "vim",                          .. defines a preffered text editor command for text edit.
    "error_file" : "error.log"                      .. path to error file where additional info about errors are dumped.
  },
  "paths" : {
    "db_file_path"  : "out/db.sql",                 .. path to SQL database file
    "work_dir" : "work/",                           .. path to work directory for aux. files
    "bard_command" : "lib/bard/target/debug/bard",  .. path to bard command line tool
    "output_dir" : "out/"                           .. path to output directory for PDF
  },
  "google": {
    "api_key" : "",                                 .. app uses this API key to search Google and scrap lyrics & chords
    "search_engine" : ""                            .. (same)
  },
  "ai"  :  {                                   
    "model" : "",                                   .. name of an AI model to process lyrics into desired format
    "api_key" : ""                                  .. an Open AI API key
  },
  "songbook" : {
    "title" : "",                                   .. front site title for a PDF songbook
    "subtitle" : "",                                .. front site subtitle for a PDF songbook
    "note" : ""                                     .. small footnote on a bottom of PDF songbook
  }
}
```
