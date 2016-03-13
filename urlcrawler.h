#ifndef URLCRAWLER_H
#define	URLCRAWLER_H

#include <string>
#include <map>

using namespace std;

//-----------------------------------------------------------------------------
class UrlCrawler
{
	private:
		map<string, string>		_links;

		virtual	unsigned int	extractLinks(const string html);

	public:
								UrlCrawler();
		virtual					~UrlCrawler();

		virtual	bool			crawlHtml(const string url, const string targetDirName);
};
#endif  //  URLCRAWLER_H