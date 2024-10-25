#include "Database.h"
#include "Song.h"
#include "json/json.hpp"
#include <iostream>
#include <string>

using json_t = nlohmann::json;

int main() {
	

    Database* data = new Database();
	data->loadJsonFile("database.json");
	/*
	Song song1 = Song("Call of the mountains", "Eluveitie");
	data->addSong(&song1);
	Song song2 = Song("Kometa", "J. Nohavica");
	data->addSong(&song2);
	Song song3 = Song("Tesinska", "Kachna a Zodiac");
	data->addSong(&song3);
	Song song4 = Song("Dalko Daleka", "Hop Trop");
	data->addSong(&song4);
	Song song5 = Song("At", "Zrni");
	data->addSong(&song5);
	data->saveJsonFile();
*/

    delete data;


    return 0;
}
