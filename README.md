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

## HOW TO INSTALL
### DEPENDENCIES
  * Django 5.2 (or newer)
  * Flutter 3.29 (or newer)
  * g++
  * make 4.3 (or newer)
  * curl 8.5.0 (or newer) (TODO not necessary now)
  * python3.12 (or newer)
### RUN A BACKEND
  ```
  # activate virtual environment
  source env/bin/activate
  # run a server
  cd django-server && python3 manage.py runserver
  ```

## DEVELOPMENT NOTES
Please note that this repository was initially used for CLI application written in C++, providing similar functionality
