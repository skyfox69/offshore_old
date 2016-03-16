#include "urlcrawler.h"
#include "options.h"
#include "urlloader.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
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
string UrlCrawler::unifyLink(const string link)
{
	string		linkOut(link);
	size_t		pos    (0);

	//  remove http://
	while ((pos = linkOut.find("http://", pos)) != string::npos) {
		linkOut.replace(pos, 7, "");
	}

	//  remove https://
	while ((pos = linkOut.find("https://", pos)) != string::npos) {
		linkOut.replace(pos, 8, "");
	}

	return linkOut;
}

//-----------------------------------------------------------------------------
string UrlCrawler::getFileNameFromLink(const string link)
{
	const char*	pChar   (nullptr);
	string		fileName(link);
	size_t		pos     (0);

	//  remove http://
	while ((pos = fileName.find("http://", pos)) != string::npos) {
		fileName.replace(pos, 7, "");
	}

	//  remove https://
	pos = 0;
	while ((pos = fileName.find("https://", pos)) != string::npos) {
		fileName.replace(pos, 8, "");
	}

	//  replace / with _
	replace(fileName.begin(), fileName.end(), '/', '_');

	//  replace . with _
	replace(fileName.begin(), fileName.end(), '.', '_');
	
	

	//  append .html if not set
	pChar = fileName.c_str() + fileName.size() - 5;
	if (strcasecmp(pChar, ".html") != 0) {
		fileName += ".html";
	}

	return fileName;
}

//-----------------------------------------------------------------------------
bool UrlCrawler::exchangeLinks()
{
	string	link;
	string	fileName;
	string	html;

	//  for each file
	for (auto& entry : _links) {
		link     = entry.first;
		fileName = entry.second._fileName;

		ifstream	inFile(fileName, ifstream::binary);

		inFile >> html;
		inFile.close();




	}  //  for (auto& link : _links)

	return true;
}

//-----------------------------------------------------------------------------
unsigned int UrlCrawler::extractLinks(const string html, const unsigned char depth, vector<string>& myLinks)
{
	size_t		size   (html.size()+1);
	char*		pBuffer(new char[size]);
	char*		pStart (pBuffer);
	char*		pStartS(pBuffer);
	char*		pEnd   (nullptr);
	string		href;
	char		match  ('\"');

	memcpy(pBuffer, html.c_str(), size-1);
	pBuffer[size-1] = 0;

	//  ignore links within head section
	if (!_pOptions->_parseHeader) {
		pStart = strstr(pStart, "<body");
		if (pStart == nullptr) {
			return 0;
		}
	}

	//  get first href
	pStartS = pStart;
	pStart  = strstr(pStart, "href=\"");
	if (pStart == nullptr) {
		pStart = strstr(pStartS, "href='");
		match = '\'';
	}

	//  get each href
	while (pStart != nullptr) {
		pStart += 6;
		pEnd  = strchr(pStart, match);
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

			//  get next href
			match  = '"';
			pStart = strstr(++pEnd, "href=\"");
			if (pStart == nullptr) {
				pStart = strstr(pEnd, "href='");
				match = '\'';
			}

		}  //  if (pEnd != nullptr)
	}  //  while ((pStart = strstr(pStart, "href")) != nullptr)

	delete[] pBuffer;

	return _links.size();
}

//-----------------------------------------------------------------------------
unsigned int UrlCrawler::extractIFrames(const string html, const unsigned char depth, vector<string>& myLinks)
{
	size_t		size   (html.size()+1);
	char*		pBuffer(new char[size]);
	char*		pStart (pBuffer);
	char*		pStartS(pBuffer);
	char*		pEnd   (nullptr);
	char*		pEndS  (nullptr);
	string		href;
	char		match  ('\"');

	memcpy(pBuffer, html.c_str(), size-1);
	pBuffer[size-1] = 0;

	//  ignore links within head section
	if (!_pOptions->_parseHeader) {
		pStart = strstr(pStart, "<body");
		if (pStart == nullptr) {
			return 0;
		}
	}

	//  get first iframe
	pStart = strstr(pStart, "<iframe");
	while (pStart != nullptr) {
		pStart += 7;
		pEnd = strstr(pStart, "</iframe>");
		if (pEnd != nullptr) {
			*pEnd   = 0;
			pStartS = pStart;
			match   = '"';
			pStartS = strstr(pStart, "src=\"");
			if (pStartS == nullptr) {
				pStartS = strstr(pStart, "src='");
				match = '\'';
			}

			if (pStartS != nullptr) {
				pStartS += 5;
				pEndS = strchr(pStartS, match);
				if (pEndS != nullptr) {
					*pEndS = 0;
					if (pEndS[-1] == '/') {
						pEndS[-1] = 0;
					}
					href = pStartS;

					if (isValidLink(href)) {
						if (_links.count(href) <= 0) {
							_links[href]._depth = depth;
							myLinks.push_back(href);
						}
					}
				}  //  if (pEndS != nullptr)
			}  //  if (pStartS != nullptr)

			pStart = pEnd + 9;
		}  //  if (pEnd != nullptr)

		pStart = strstr(pStart, "<iframe");
	}  //  while (pStart != nullptr)

	delete[] pBuffer;

	return _links.size();
}

//-----------------------------------------------------------------------------
bool UrlCrawler::crawlHtmlRecursive(const string url, const string targetDirName, const unsigned char depth)
{
	vector<string>	myLinks;

	//  load url as text (page-source))
	string		html(_pUrlLoader->readHtml(url));

	//  write to local file
	if (!_pOptions->_simulate) {
		//  generate file name
		if (_links[url]._fileName.empty()) {
			_links[url]._fileName = "./" + targetDirName + "/" + getFileNameFromLink(url);
		}

		ofstream	outFile(_links[url]._fileName, ofstream::binary);

		outFile.write(html.c_str(), html.size());
		outFile.close();
	}
	fprintf(stderr, "size of %s: %ld\n", url.c_str(), html.size());

	//  extract links
	extractLinks  (html, depth, myLinks);
	extractIFrames(html, depth, myLinks);

	//  recursive parse links
	if (depth < _pOptions->_recurseDepth) {
		for (auto& link : myLinks) {
			crawlHtmlRecursive(link, targetDirName, depth + 1);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
bool UrlCrawler::crawlHtml(const string url, const string targetDirName)
{
	fprintf(stderr, "crawling: %s => %s\n", url.c_str(), targetDirName.c_str());

	//  initialize lists
	_links.clear();
	_images.clear();

	if (_pOptions->_linkFileName.empty()) {
		if (!_pOptions->_simulate) {
			//  check for existing target
			struct stat		st = {0};

			if (stat(("./" + targetDirName).c_str(), &st) == -1) {
				fprintf(stderr, "creating target directory\n");
				mkdir(("./" + targetDirName).c_str(), 0775);
			}
		}

		//  insert origin link
		_links[url]._depth    = 0;
		_links[url]._fileName = "./" + targetDirName + "/index.html";

		//  get links recursive (if wanted)
		crawlHtmlRecursive(url, targetDirName, 0);

		if (!_pOptions->_simulate) {
			//  get content from last iteration
			for (auto& link : _links) {
				if (link.second._fileName.empty()) {
					//  load url as text (page-source))
					string		html(_pUrlLoader->readHtml(link.first));

					fprintf(stderr, ">>> %s\n", link.first.c_str());

					//  generate file name
					if (_links[link.first]._fileName.empty()) {
						_links[link.first]._fileName = "./" + targetDirName + "/" + getFileNameFromLink(link.first);
					}

					ofstream	outFile(_links[link.first]._fileName, ofstream::binary);

					outFile.write(html.c_str(), html.size());
					outFile.close();

				}  //  if (link.second._fileName.empty())
			}  //  for (auto& link : _links)
		}  //  if (!_pOptions->_simulate)
	} else {
		ifstream	inFile(_pOptions->_linkFileName);
		string		link;
		string		fileName;
		string		depth;

		fprintf(stderr, "  reading links from %s\n", _pOptions->_linkFileName.c_str());

		//  read file line by line
		while (getline(inFile, depth, ',')) {
			getline(inFile, link, ',');
			getline(inFile, fileName);

			_links[link]._depth    = (unsigned char) atoi(depth.c_str());
			_links[link]._fileName = "./" + targetDirName + "/" + fileName;
		}

		inFile.close();
	}

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
	}

	//  process files, exchange links
	if (!_pOptions->_simulate) {
		exchangeLinks();
	}


	return true;
}