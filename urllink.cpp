#include "urllink.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>

//-----------------------------------------------------------------------------
UrlLink::UrlLink()
{}

//-----------------------------------------------------------------------------
UrlLink::~UrlLink()
{}

//-----------------------------------------------------------------------------
UrlLink& UrlLink::operator =(UrlLink link)
{
	_depth          = link._depth;
	_fileName       = link._fileName;
	_fileNameIncDir = link._fileNameIncDir;
	_link           = link._link;
	_originalLink   = link._originalLink;

	return *this;
}

//-----------------------------------------------------------------------------
ostream& operator<<(ostream& stream, const UrlLink& link)
{
	stream << ((short) link._depth) << "," << link._link << "," << link._originalLink << "," << link._fileName << "," << link._fileNameIncDir;
	return stream;
}

//-----------------------------------------------------------------------------
istream& operator>>(istream& stream, UrlLink& link)
{
	string	depth;

	getline(stream, depth, ',');
	getline(stream, link._link, ',');
	getline(stream, link._originalLink, ',');
	getline(stream, link._fileName, ',');
	getline(stream, link._fileNameIncDir);
	link._depth = (unsigned char) atoi(depth.c_str());
	return stream;
}