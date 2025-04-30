# SONGBOOK APPLICATION
This repository contains a multiplatform songbook application, with Django server as a database backend, flutter app (written in Dart) for frontend.
The application usage:
  * Store songs into database (title, artist)
  * Edit/Delete songs in database
  * Sort individual songs based on given criteria
  * Manage multiple users via email auth.
    * Each user has its own database field
    * password reset via mail
  * Automatically find chords on the verified websites and provide a proxy to it

Github URL: https://github.com/petrcechura/Song-book

## HOW TO INSTALL
### DEPENDENCIES
  * Django 5.2 (or newer)
  * Flutter 3.29 (or newer)
  * g++
  * make 4.3 (or newer)
  * curl 8.5.0 (or newer) (TODO not necessary now)
  * python3.12 (or newer)
### RUN A BACKEND (linux)
  ```
  # activate virtual environment
  source env/bin/activate
  # run a server
  cd django-server && python3 manage.py runserver
  ```
### RUN A FLUTTER APP
 
  * Open VS Code
  * install `Flutter` extension
  * run `flutter/lib/main.dart`
## DEVELOPMENT NOTES
Please note that this repository was initially used for CLI application written in C++, providing similar functionality, but locally, without server. It's purpose was rather for C++ language training, hence many features are handwritten instead of using available libraries (that's the main reason for using raw C++ database + json instead of SQL-based database).
The CLI application is still part of this repository and it is planned to synchronize its functionality with Django-backend. However right now, its database is rather simple (JSON file only), thus this task remains for the future development.
### GOALS FOR CLI APP

- [x] CRUD operations over song database
- [x] Features to sort/backup database
- [x] LaTeX conversion feature
- [ ] Implement SQL table instead of raw JSON
- [ ] Add tasks for fetching/pushing songs from/to Django-backend
- [ ] Implement user auth. system

