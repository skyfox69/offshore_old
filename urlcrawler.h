#ifndef URLCRAWLER_H
#define	URLCRAWLER_H

#include <string>
#include <map>
#include <vector>

using namespace std;

class	Options;
class	UrlLoader;

//-----------------------------------------------------------------------------
struct Link
{
	string				_fileName;
	unsigned char		_depth = 0;
};

//-----------------------------------------------------------------------------
class UrlCrawler
{
	private:
		Options*					_pOptions;
		UrlLoader*					_pUrlLoader;
		map<string, Link>			_links;
		map<string, Link>			_images;

		virtual bool				isValidImage(const string link);
		virtual bool				isValidLink (const string link);
		virtual	unsigned int		extractLinks(const string html, const unsigned char depth, vector<string>& myLinks);
		virtual	bool				crawlHtmlRecursive(const string url, const string targetDirName, const unsigned char depth);

	public:
									UrlCrawler();
		virtual						~UrlCrawler();

		virtual	bool				crawlHtml(const string url, const string targetDirName);
};
#endif  //  URLCRAWLER_H