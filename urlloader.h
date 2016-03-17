#ifndef URLLOADER_H
#define	URLLOADER_H

#include <string>

using namespace std;

//-----------------------------------------------------------------------------
class UrlLoader
{
	private:
		static	UrlLoader*	_pInstance;

	protected:
							UrlLoader();
		virtual	bool		read(const string url, ostream& oStream);

	public:
		virtual				~UrlLoader();
		static	UrlLoader*	getInstance();

		virtual	string		readHtml(const string url);
		virtual	string		downloadImage(const string url, const string fileName);
};
#endif  //  URLLOADER_H