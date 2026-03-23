#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "CollectionTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "SongEditorServer.hpp"

int CollectionTask::Execute(char command)
{
	switch(parent->getState())  {
		case SongBookApp::app_state_t::SONG_BROWSE:

			break;
		case SongBookApp::app_state_t::COLLECTION_BROWSE:
			switch(command)  {
				case 'a': createCollection();
					break;
				case 'd': deleteCollection();
					break;
				case 'o': addToCollection();
					break;
			}
			break;
	}
    
	return 0;
}

void CollectionTask::createCollection()
{
	windows["Log Screen"]->Clear();

	std::string name;

	windows["Log Screen"]->Print("Type a collection name:");
	name = windows["Log Screen"]->GetString(30);

	if (name.empty())  {
		windows["Log Screen"]->Print("Invalid Name inserted, terminating...");
		return;
	}
	else if (parent->getDatabase()->collectionExists(name))  {
		windows["Log Screen"]->Print("Collection with this name already exists!");
		return;
	}
	else {
		int exit = parent->getDatabase()->addCollection(name);

		switch(exit)  {
			case 0:
				windows["Log Screen"]->Print(std::format("Collection '{}' created...", name));
				break;
			case 1:
				windows["Log Screen"]->Print("Internal error in sqlite database!");
				break;
			case 2:
      			windows["Log Screen"]->Print("Error while trying to get incremental ID for new item!");
				break;
			case 3:
				windows["Log Screen"]->Print(std::format("Collection '{}' already exists!", name));
				break;
			default:
				windows["Log Screen"]->Print("Unexpected error happened when creating new collection...");
				break;

		}
	}

	nlohmann::json song = nlohmann::json();

	// update the song count in config json
  	nlohmann::json collections = parent->getDatabase()->getCollections();
  	SongBookUtils::setConfigItem("workspace/collection_count", std::to_string(collections.size()));
  	SongBookUtils::setConfigJson("workspace/collections", collections);
}

void CollectionTask::deleteCollection()
{
	windows["Log Screen"]->Clear();

	int id = stoi(SongBookUtils::getInstance()->getConfigItem("workspace/current_collection_id", "-1"));

	if (parent->getDatabase()->collectionExists(id))  {
		windows["Log Screen"]->Print("Are you sure you want to delete this collection? (y/n)");

		std::string choice = windows["Log Screen"]->GetString();

		if (choice == "y")  {
			int error = parent->getDatabase()->removeCollection(id);
			if (!error)  {
				windows["Log Screen"]->Print("Collection successfully deleted...");
				
			}
			else {
				windows["Log Screen"]->Print("Error occured when deleting a song...");
			}
		}
	}
	else {
		windows["Log Screen"]->Print(std::format("Song with id '{}' does not exist!", id));
	}

	// update the song count in config json
  	nlohmann::json collections = parent->getDatabase()->getCollections();
  	SongBookUtils::setConfigItem("workspace/collection_count", std::to_string(collections.size()));
  	SongBookUtils::setConfigJson("workspace/collections", collections);
}

void CollectionTask::selectCollection()
{
}

void CollectionTask::addToCollection()
{	
	int collection_id = stoi(SongBookUtils::getConfigItem("workspace/current_collection_id"));
	auto marked_songs = SongBookUtils::getConfigJson("workspace/marked_songs");

	if (!marked_songs.is_array())  {
		SongBookUtils::printError("Array not found under `workspace/marked_songs`!");
		return;
	}

	windows["Log Screen"]->Clear();
	for (std::string song_id : marked_songs)  {
		parent->getDatabase()->addSongToCollection(stoi(song_id), collection_id);
		windows["Log Screen"]->Print(std::format("Song ({}) added to ({})", song_id, collection_id));
	}

}

void CollectionTask::removeFromCollection()
{
	int collection_id = stoi(SongBookUtils::getConfigItem("workspace/current_collection_id"));
	auto marked_songs = SongBookUtils::getConfigJson("workspace/marked_songs");

	if (!marked_songs.is_array())  {
		SongBookUtils::printError("Array not found under `workspace/marked_songs`!");
		return;
	}

	windows["Log Screen"]->Clear();
	for (std::string song_id : marked_songs)  {
		parent->getDatabase()->removeSongFromCollection(stoi(song_id), collection_id);
		windows["Log Screen"]->Print(std::format("Song ({}) removed from ({})", song_id, collection_id));
	}
}