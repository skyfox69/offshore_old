#ifndef URLLINK_H
#define	URLLINK_H

#include <string>
#include <fstream>

using namespace std;

//-----------------------------------------------------------------------------
struct UrlLink
{
	string				_link;
	string				_originalLink;
	string				_fileNameIncDir;
	string				_fileName;
	unsigned char		_depth = 0;


						UrlLink();
	virtual				~UrlLink();

	virtual	UrlLink&	operator=(UrlLink link);

	friend	ostream&	operator<<(ostream& stream, const UrlLink& link);
	friend	istream&	operator>>(istream& stream, UrlLink& link);
};
#endif  //  URLLINK_H