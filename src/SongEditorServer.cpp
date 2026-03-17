#include <string>
#include <vector>
#include "SongEditorServer.hpp"



SongEditorServer::SongEditorServer(std::wstring str, bool* running)
{	
	this->content = str2vec(str);
	this->running = running;
	this->line_cnt = str->size();
}

std::vector<std::wstring> SongEditorServer::str2vec(std::wstring& txt)
{
	std::vector<std::wstring> v = new std::vector<std::wstring>();
	std::wstring segment;

	while(std::getline(txt, segment, '\n'))  {
   		v.push_back(segment);
	}

	return v;
}

std::wstring SongEditorServer::vec2str(std::vector<std::wstring>& vec)
{	
	std::wstring str;

	for (auto const s : vec)  {
		str.push_back(s)
		str.push_back('\n');
	}

	return str;
}

void SongEditorServer::Edit(wchar_t c)
{
	switch(c)
	{	
		// Backspace character
		case 127:  	if (cursor == 0) 
						if (line != 0)  {
							deleteLine();
							//moveAt(line, content->at(line).size()-1);
						}
					else
						deleteChar();
					break;

		case '\n': 	
					createLine();
					break;
		// case arrow left
					moveLeft();
					break;
		// case arrow right
					moveRight();
					break;
		// case arrow up
					moveUp();
					break;
		// case arrow down
					moveDown();
					break;
		// ESC character
		case 27:   exitEditor();
		default: insertChar(c);
	}

	return vec2str(this->content);
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
	this->content[cursor[0]].insert(cursor[1], c);
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

void SongEditorServer::createLine()
{
	
}

void SongEditorServer::moveRight()
{
	if (cursor[1] < content[cursor[0]].size())  {
			cursor[1]++;
	}
}
void SongEditorServer::moveLeft()
{
	if (cursor[1] > 0) {
		cursor[1]--;
	}
}
void SongEditorServer::moveUp()
{
	if (cursor[0] > 0)  {
		cursor[0]--;

		if (content[cursor[0]].size() < cursor[1])  {
			cursor[1] = content[cursor[0]].size() - 1;
		}
	}
}
void SongEditorServer::moveDown()
{
	if (cursor[0] < line_cnt)  {
		cursor[1]++;

		if (content[cursor[0]].size() < cursor[1])  {
			cursor[1] = content[cursor[0]].size() - 1;
		}
	}
}
void SongEditorServer::moveAt(int line, int pos)
{
	if (line < line_cnt) {
		if (pos < content[line].size())
		cursor[0] = line;
		cursor[1] = pos;
	}
}	