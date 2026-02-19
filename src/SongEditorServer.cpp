#include <string>
#include "SongEditorServer.hpp"



SongEditorServer::SongEditorServer(std::wstring* content, bool* running)
{
	this->content = content;
	this->running = running;
}

void SongEditorServer::Edit(wchar_t c)
{
	switch(c)
	{	
		// Backspace character
		case 127:  deleteChar();
					 break;
		
		// ESC character
		case 27:   exitEditor();
		default: insertChar(c);
	}
}

void SongEditorServer::deleteLine()
{

}
void SongEditorServer::deleteChar()
{
	if (!this->content->empty())  {
		this->content->pop_back();
	}
}
void SongEditorServer::deleteChords()
{

}
void SongEditorServer::insertChar(wchar_t c)
{
	this->content->push_back(c);
}
void SongEditorServer::insertChord()
{

}
void SongEditorServer::transpose(int i)
{

}
void SongEditorServer::exitEditor()
{
	*running = false;
}