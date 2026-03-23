#include <string>
#include <iostream>
#include <iomanip>
#include <format>
#include "FilterTask.h"
#include "SongBookApp.h"
#include "SongBookUtils.h"
#include "SongEditorServer.hpp"

int FilterTask::Execute(char command)
{	
	switch(parent->getState())  {
		case SongBookApp::app_state_t::SONG_BROWSE:
      		switch(command)  {
        		case 'f': filterData();
						  break;
        		default: return 1;
      		}
			break;
		case SongBookApp::app_state_t::COLLECTION_BROWSE:
			switch(command)  {
				// enter
        		case 'f': filterCollection();
						  break;
        		default: return 1;
      		}
			break;
	}

    return 0;
}

void FilterTask::filterData()
{
	windows["Log Screen"]->Clear();

	windows["Log Screen"]->Print("Type a filtering pattern:");
	std::string pattern = windows["Log Screen"]->GetString(10);

	SongBookUtils::setConfigItem("workspace/filter_pattern", pattern);

	if (pattern.empty())  {
		windows["Log Screen"]->Print(std::format("Filter pattern removed.", pattern));
	} else {
		windows["Log Screen"]->Print(std::format("Filter pattern '{}' applied.", pattern));
	}

  	nlohmann::json songs = parent->getDatabase()->getJson();
	nlohmann::json collections = parent->getDatabase()->getCollections();
	std::string song_count = std::to_string(songs.size());
	std::string collection_count = std::to_string(collections.size());
  	SongBookUtils::setConfigJson("workspace/songs", songs);
	SongBookUtils::setConfigJson("workspace/collections", collections);
  	SongBookUtils::setConfigJson("workspace/song_count", song_count);
	SongBookUtils::setConfigJson("workspace/collection_count", collection_count);
}

void FilterTask::sortList()
{

}

void FilterTask::filterCollection()
{
	std::string collection_id = SongBookUtils::getConfigItem("workspace/current_collection_id", "-1");

	if (collection_id != "-1")  {
		SongBookUtils::setConfigItem("workspace/filter_collection", collection_id);
	}

  	nlohmann::json songs = parent->getDatabase()->getJson();
	nlohmann::json collections = parent->getDatabase()->getCollections();
	std::string song_count = std::to_string(songs.size());
	std::string collection_count = std::to_string(collections.size());
  	SongBookUtils::setConfigJson("workspace/songs", songs);
	SongBookUtils::setConfigJson("workspace/collections", collections);
  	SongBookUtils::setConfigJson("workspace/song_count", song_count);
	SongBookUtils::setConfigJson("workspace/collection_count", collection_count);
}

void FilterTask::selectCollection()
{	
	std::string collection = SongBookUtils::getConfigItem("workspace/current_collection_id", "0");

	windows["Log Screen"]->Print(std::format("Collection '{}' selected.", collection));
}