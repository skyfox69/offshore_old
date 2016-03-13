#include "urlcrawler.h"
#include "options.h"
#include "urlloader.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <limits.h>

//-----------------------------------------------------------------------------
UrlCrawler::UrlCrawler()
{
	_pOptions   = Options::getInstance();
	_pUrlLoader = UrlLoader::getInstance();
}

//-----------------------------------------------------------------------------
UrlCrawler::~UrlCrawler()
{}

//-----------------------------------------------------------------------------
bool UrlCrawler::isValidImage(const string link)
{
	const char*		pText  (link.c_str());
	bool			isValid(false);

	//  check on image
	if (strlen(pText) > 4) {
		pText = &(pText[strlen(pText) - 4]);
		isValid |= (strcasecmp(pText, ".jpg") == 0);
		isValid |= (strcasecmp(pText, ".png") == 0);
		isValid |= (strcasecmp(pText, ".bmp") == 0);
		isValid |= (strcasecmp(pText, ".gif") == 0);
	}

	//  check for exclusion
	if (isValid) {

		


	}

	return isValid;
}

//-----------------------------------------------------------------------------
bool UrlCrawler::isValidLink(const string link)
{
	bool	isValid (true);

	//  check positive
	if (_pOptions->_includes.size() > 0) {
		isValid = false;
		for (auto& text : _pOptions->_includes) {
			if (link.find(text.second) != string::npos) {
				isValid = true;
				break;
			}
		}
	}

	//  check negative
	if (isValid && (_pOptions->_excludes.size() > 0)) {
		for (auto& text : _pOptions->_excludes) {
			if (link.find(text.second) != string::npos) {
				isValid = false;
				break;
			}
		}
	}

	//  check pseudo-link
	if (link.find("#") != string::npos) {
		isValid = false;
	}

	return isValid;
}

//-----------------------------------------------------------------------------
unsigned int UrlCrawler::extractLinks(const string html, const unsigned char depth, vector<string>& myLinks)
{
	size_t		size    (html.size()+1);
	char*		pBuffer (new char[size]);
	char*		pStart  (pBuffer);
	char*		pEnd    (nullptr);
	string		href;

	memcpy(pBuffer, html.c_str(), size-1);
	pBuffer[size-1] = 0;

	//  ignore links within head section
	if (!_pOptions->_parseHeader) {
		pStart = strstr(pStart, "<body");
		if (pStart == nullptr) {
			return 0;
		}
	}

	//  get each href
	while ((pStart = strstr(pStart, "href=\"")) != nullptr) {
		pStart += 6;
		pEnd = strchr(pStart, '"');
		if (pEnd == nullptr) {
			pEnd = strchr(pStart, '\'');
		}
		if (pEnd != nullptr) {
			*pEnd = 0;
			if (pEnd[-1] == '/') {
				pEnd[-1] = 0;
			}
			href  = pStart;

			//  valid image
			if (isValidImage(href)) {
				if (_images.count(href) <= 0) {
					_images[href]._fileName = href;
					_images[href]._depth    = depth;
				}
			}
			//  valid link
			else if (isValidLink(href)) {
				if (_links.count(href) <= 0) {
					_links[href]._depth = depth;
					myLinks.push_back(href);
				}
			}

			pStart = ++pEnd;

		}  //  if (pEnd != nullptr)
	}  //  while ((pStart = strstr(pStart, "href")) != nullptr)

	delete[] pBuffer;

	return _links.size();
}

//-----------------------------------------------------------------------------
bool UrlCrawler::crawlHtmlRecursive(const string url, const string targetDirName, const unsigned char depth)
{
	vector<string>	myLinks;

	//  load url as text (page-source))
	string		html(_pUrlLoader->readHtml(url));

	fprintf(stderr, "size of %s: %ld\n", url.c_str(), html.size());

	//  extract links
	extractLinks(html, depth, myLinks);

	//  recursive parse links
	if (depth < _pOptions->_recurseDepth) {
		for (auto& link : myLinks) {
			crawlHtmlRecursive(link, targetDirName, depth + 1);
			//break;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
bool UrlCrawler::crawlHtml(const string url, const string targetDirName)
{
	fprintf(stderr, "crawling: %s => %s\n", url.c_str(), targetDirName.c_str());

	if (!_pOptions->_simulate) {
		//  check for existing target
		struct stat		st = {0};

		if (stat(("./" + targetDirName).c_str(), &st) == -1) {
			fprintf(stderr, "creating target directory\n");
			mkdir(("./" + targetDirName).c_str(), 0775);
		}
	}

	//  initialize lists
	_links.clear();
	_images.clear();

	//  get links recursive (if wanted)
	crawlHtmlRecursive(url, targetDirName, 0);

	//  show links if set
	if (_pOptions->_showLinks) {
		fprintf(stderr, "\nfound links:\n");
		for (auto& link : _links) {
			fprintf(stderr, "  %02d - %s\n", link.second._depth, link.first.c_str());
		}
		fprintf(stderr, "\n");
	}

	//  show images if set
	if (_pOptions->_showImages) {
		fprintf(stderr, "\nfound images:\n");
		for (auto& image : _images) {
			fprintf(stderr, "  %02d - %s\n", image.second._depth, image.first.c_str());
		}
		fprintf(stderr, "\n");
	}  //  if (pOptions->_showLinks)

	


	return true;
}