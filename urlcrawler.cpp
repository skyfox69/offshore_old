#include "urlcrawler.h"
#include "options.h"
#include "urlloader.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>

//-----------------------------------------------------------------------------
UrlCrawler::UrlCrawler()
{}

//-----------------------------------------------------------------------------
UrlCrawler::~UrlCrawler()
{}

//-----------------------------------------------------------------------------
unsigned int UrlCrawler::extractLinks(const string html)
{
	string	href;
	string	hrefPlug;
	size_t	posStart(html.find("href", 0));
	size_t	posEnd  (0);

	while (posStart != string::npos) {
		posStart = html.find("\"", posStart);
		posEnd   = html.find("\"", posStart + 1);
		href     = html.substr(posStart+1, (posEnd - posStart - 1));

		if (strncasecmp(href.c_str(), "http://", 7) == 0) {
			posStart += 7;
		}
		else if (strncasecmp(href.c_str(), "https://", 8) == 0) {
			posStart += 8;
		}
		href = html.substr(posStart+1, (posEnd - posStart - 1));

		if ((href.find("#", 0) == string::npos) && (_links.count(href) <= 0)) {
			hrefPlug = href;
			replace(hrefPlug.begin(), hrefPlug.end(), '/', '_');
			_links[href] = hrefPlug;
		}

		posStart = html.find("href", posEnd);

	}  //  while (posStart != string::npos)

	return _links.size();
}

//-----------------------------------------------------------------------------
bool UrlCrawler::crawlHtml(const string url, const string targetDirName)
{
	Options*	pOptions(Options::getInstance());
	UrlLoader*	pLoader (UrlLoader::getInstance());
	string		html;

	fprintf(stderr, "crawling: %s => %s\n", url.c_str(), targetDirName.c_str());

	if (!pOptions->_simulate) {
		//  check for existing target
		struct stat		st = {0};

		if (stat(("./" + targetDirName).c_str(), &st) == -1) {
			fprintf(stderr, "creating target directory\n");
			mkdir(("./" + targetDirName).c_str(), 0775);
		}
	}

	//  load url as text (page-source))
	html = pLoader->readHtml(url);
	fprintf(stderr, "size of %s: %ld\n", url.c_str(), html.size());

	//  extract links from html
	_links.clear();
	extractLinks(html);

	//  show links if set
	if (pOptions->_showLinks) {
		fprintf(stderr, "\nfound links:\n");

		for (auto& link : _links) {
			fprintf(stderr, "  %s\t\t%s\n", link.first.c_str(), link.second.c_str());
		}
		fprintf(stderr, "\n");

	}  //  if (pOptions->_showLinks)

	


	return true;
}