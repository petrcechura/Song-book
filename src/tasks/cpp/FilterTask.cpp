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
				case 't': sortList();
						  break;
      		}
			parent->addTaskLegend('f', "Filter songs");
			parent->addTaskLegend('t', "Sort songs");
			break;
		case SongBookApp::app_state_t::COLLECTION_BROWSE:
			switch(command)  {
				// enter
        		case 'f': filterCollection();
						  break;
      		}
			parent->addTaskLegend('f', "Filter collections");
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

// TODO this shall be replaced in future by another `settings` table, where sort options are select via simple ENTER
// (as well as other options)
void FilterTask::sortList()
{
	windows["Log Screen"]->Clear();

	windows["Log Screen"]->Print("Type a sort pattern:");
	std::string pattern = windows["Log Screen"]->GetString(10);

	if ( pattern != "ID" && pattern != "title" && pattern != "artist")  {
		windows["Log Screen"]->Print("Sort pattern must be one of: ID, title, artist");
		return;
	}

	SongBookUtils::setConfigItem("workspace/order", pattern);
	
  	nlohmann::json songs = parent->getDatabase()->getJson();
  	SongBookUtils::setConfigJson("workspace/songs", songs);
}

void FilterTask::filterCollection()
{
	std::string collection_id = SongBookUtils::getConfigItem("workspace/current_collection_id", "-1");

	if (collection_id == SongBookUtils::getConfigItem("workspace/filter_collection", ""))  {
		SongBookUtils::setConfigItem("workspace/filter_collection", "");
	}
	else if (collection_id != "-1")  {
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