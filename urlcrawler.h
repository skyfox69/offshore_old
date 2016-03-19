#ifndef URLCRAWLER_H
#define	URLCRAWLER_H

#include "urllink.h"
#include <map>
#include <vector>

using namespace std;

class	Options;
class	UrlLoader;

//-----------------------------------------------------------------------------
class UrlCrawler
{
	private:
		Options*					_pOptions;
		UrlLoader*					_pUrlLoader;
		map<string, UrlLink>		_links;
		map<string, UrlLink>		_images;

		virtual bool				isValidImage(string& link);
		virtual bool				isValidLink (string& link);
		virtual	unsigned int		extractLinks(const string html, const unsigned char depth, const string , vector<string>& myLinks);
		virtual	unsigned int		extractIFrames(const string html, const unsigned char depth, vector<string>& myLinks);
		virtual	bool				crawlHtmlRecursive(const string url, const string targetDirName, const unsigned char depth);
		virtual	string				getFileNameFromLink(const string link, const bool isImage = false);
		virtual	string				unifyLink(const string link);
		virtual	bool				exchangeLinks();

	public:
									UrlCrawler();
		virtual						~UrlCrawler();

		virtual	bool				crawlHtml(const string url, const string targetDirName);
};
#endif  //  URLCRAWLER_H