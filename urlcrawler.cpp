#include "urlcrawler.h"
#include "options.h"
#include "urlloader.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <sstream>
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
bool UrlCrawler::isValidImage(string& link)
{
	const char*		pText  (link.c_str());
	size_t			pos    (link.find("?"));
	bool			isValid(false);

	//  check for parameters
	if (pos != string::npos) {
		link  = link.substr(0, pos);
		pText = link.c_str();
	}

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
bool UrlCrawler::isValidLink(string& link)
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
	//  css
	if (!_pOptions->_includeCss && (link.find(".css") != string::npos)) {
		isValid = false;
	}
	//  js
	if (!_pOptions->_includeJs && (link.find(".js") != string::npos)) {
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
	pos = 0;
	while ((pos = linkOut.find("https://", pos)) != string::npos) {
		linkOut.replace(pos, 8, "");
	}

	return linkOut;
}

//-----------------------------------------------------------------------------
string UrlCrawler::getFileNameFromLink(const string link, const bool isImage)
{
	const char*	pChar   (nullptr);
	string		fileName(unifyLink(link));

	//  replace / with _
	replace(fileName.begin(), fileName.end(), '/', '_');

	//  replace . with _
	replace(fileName.begin(), fileName.end(), '.', '_');
	
	

	//  append .html if not set
	pChar = fileName.c_str() + fileName.size() - 5;
	if (!isImage && (strcasecmp(pChar, ".html") != 0)) {
		fileName += ".html";
	}

	return fileName;
}

//-----------------------------------------------------------------------------
bool UrlCrawler::exchangeLinks()
{
	string			link;
	string			fileName;
	string			html;
	size_t			pos  (0);
	unsigned int	count(0);

	//  for each file
	for (auto& entry : _links) {
		link     = entry.first;
		fileName = entry.second._fileNameIncDir;

		if (!fileName.empty()) {
			ifstream		inFile(fileName, ifstream::binary);
			stringstream	sStream;

			fprintf(stderr, "processing: %s\n", fileName.c_str());

			//  read original file
			sStream << inFile.rdbuf();
			inFile.close();
			html = sStream.str();

			//  for each link
			for (auto& repEntry : _links) {
				//  skip empty links
				if (repEntry.second._originalLink.empty())		continue;

				count = 0;
				pos   = 0;
				while ((pos = html.find((repEntry.second._originalLink + "\""), pos)) != string::npos) {
					html.replace(pos, repEntry.second._originalLink.length(), repEntry.second._fileName);
					pos += repEntry.second._fileName.length();
					++count;
				}
				pos   = 0;
				while ((pos = html.find((repEntry.second._originalLink + "'"), pos)) != string::npos) {
					html.replace(pos, repEntry.second._originalLink.length(), repEntry.second._fileName);
					pos += repEntry.second._fileName.length();
					++count;
				}

				if (count > 0) {
					fprintf(stderr, "L:  %02d x %s\n", count, repEntry.second._originalLink.c_str());
				}
			}  //  for (auto& repEntry : _links)

			//  for each image
			for (auto& repEntry : _images) {
				//  skip empty links
				if (repEntry.second._originalLink.empty())		continue;

				count = 0;
				pos   = 0;
				while ((pos = html.find((repEntry.second._originalLink + "\""), pos)) != string::npos) {
					html.replace(pos, repEntry.second._originalLink.length(), repEntry.second._fileName);
					pos += repEntry.second._fileName.length();
					++count;
				}
				pos   = 0;
				while ((pos = html.find((repEntry.second._originalLink + "'"), pos)) != string::npos) {
					html.replace(pos, repEntry.second._originalLink.length(), repEntry.second._fileName);
					pos += repEntry.second._fileName.length();
					++count;
				}
				pos   = 0;
				while ((pos = html.find((repEntry.second._originalLink + "?"), pos)) != string::npos) {
					html.replace(pos, repEntry.second._originalLink.length(), repEntry.second._fileName);
					pos += repEntry.second._fileName.length();
					++count;
				}

				if (count > 0) {
					fprintf(stderr, "I:  %02d x %s\n", count, repEntry.second._originalLink.c_str());
				}
			}  //  for (auto& repEntry : _images)

			//  write modified file
			ofstream	outFile(fileName, ofstream::binary);

			outFile.write(html.c_str(), html.size());
			outFile.close();

		}  //  if (!fileName.empty())
	}  //  for (auto& link : _links)

	return true;
}

//-----------------------------------------------------------------------------
unsigned int UrlCrawler::extractLinks(const string html, const unsigned char depth, const string tag, vector<string>& myLinks)
{
	size_t		size   (html.size()+1);
	char*		pBuffer(new char[size]);
	char*		pStart (pBuffer);
	char*		pStartS(pBuffer);
	char*		pEnd   (nullptr);
	string		href;
	string		hrefOrig;
	string		sTag   (tag + "=\"");
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
	pStart  = strstr(pStart, sTag.c_str());
	if (pStart == nullptr) {
		sTag   = tag + "='";
		pStart = strstr(pStartS, sTag.c_str());
		match = '\'';
	}

	//  get each href
	while (pStart != nullptr) {
		pStart += sTag.size();
		pEnd  = strchr(pStart, match);
		if (pEnd != nullptr) {
			*pEnd    = 0;
			hrefOrig = pStart;
			if (pEnd[-1] == '/') {
				pEnd[-1] = 0;
			}
			href = pStart;

			//sfprintf(stderr, "~~ %s\n~~ %s\n", hrefOrig.c_str(), href.c_str());

			//  valid image
			if (isValidImage(href)) {
				if (_images.count(href) <= 0) {
					_images[href]._depth        = depth;
					_images[href]._link         = href;
					_images[href]._originalLink = href;
				}
			}
			//  valid link
			else if (isValidLink(href)) {
				if (_links.count(href) <= 0) {
					_links[href]._depth        = depth;
					_links[href]._link         = href;
					_links[href]._originalLink = hrefOrig;
					myLinks.push_back(href);
				}
			}

			//  get next href
			match  = '"';
			sTag = tag + "=\"";
			pStart = strstr(++pEnd, sTag.c_str());
			if (pStart == nullptr) {
				sTag   = tag + "='";
				pStart = strstr(pEnd, sTag.c_str());
				match = '\'';
			}

		}  //  if (pEnd != nullptr)
	}  //  while (pStart != nullptr)

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
	string		hrefOrig;
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
					hrefOrig = pStartS;
					*pEndS = 0;
					if (pEndS[-1] == '/') {
						pEndS[-1] = 0;
					}
					href = pStartS;

					if (isValidLink(href)) {
						if (_links.count(href) <= 0) {
							_links[href]._depth        = depth;
							_links[href]._link         = href;
							_links[href]._originalLink = hrefOrig;
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
			_links[url]._fileName       = getFileNameFromLink(url);
			_links[url]._fileNameIncDir = "./" + targetDirName + "/" + _links[url]._fileName;
		}

		ofstream	outFile(_links[url]._fileNameIncDir, ofstream::binary);

		outFile.write(html.c_str(), html.size());
		outFile.close();
	}
	fprintf(stderr, "size of %s: %ld\n", url.c_str(), html.size());

	//  extract links
	extractLinks  (html, depth, "href", myLinks);
	extractLinks  (html, depth, "src", myLinks);
	//extractIFrames(html, depth, myLinks);

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

	if (_pOptions->_linkFileNameRead.empty()) {
		if (!_pOptions->_simulate) {
			//  check for existing target
			struct stat		st = {0};

			if (stat(("./" + targetDirName).c_str(), &st) == -1) {
				fprintf(stderr, "creating target directory\n");
				mkdir(("./" + targetDirName).c_str(), 0775);
			}
		}

		//  insert origin link
		_links[url]._depth          = 0;
		_links[url]._link           = url;
		_links[url]._originalLink   = url;
		_links[url]._fileName       = "index.html";
		_links[url]._fileNameIncDir = "./" + targetDirName + "/index.html";

		//  get links recursive (if wanted)
		crawlHtmlRecursive(url, targetDirName, 0);

		if (!_pOptions->_simulate) {
			unsigned int	count(1);

			//  get content from last iteration
			for (auto& link : _links) {
				if (link.second._fileName.empty()) {
					//  load url as text (page-source))
					string		html(_pUrlLoader->readHtml(link.first));

					fprintf(stderr, ">>> %s\n", link.first.c_str());

					//  generate file name
					if (_links[link.first]._fileName.empty()) {
						_links[link.first]._fileName       = getFileNameFromLink(link.first);
						_links[link.first]._fileNameIncDir = "./" + targetDirName + "/" + _links[link.first]._fileName;
					}

					ofstream	outFile(_links[link.first]._fileNameIncDir, ofstream::binary);

					outFile.write(html.c_str(), html.size());
					outFile.flush();
					outFile.close();

				}  //  if (link.second._fileName.empty())
			}  //  for (auto& link : _links)

			//  download images
			for (auto& image : _images) {
				if (image.second._fileName.empty()) {
					//  generate file name
					if (_images[image.first]._fileName.empty()) {
						_images[image.first]._fileName       = getFileNameFromLink(image.first, true);
						_images[image.first]._fileNameIncDir = "./" + targetDirName + "/" + _images[image.first]._fileName;
					}

					fprintf(stderr, "### (%d/%d) %s\n", count++, _images.size(), image.first.c_str());

					//  download image
					_pUrlLoader->downloadImage(image.first, _images[image.first]._fileNameIncDir);

				}  //  if (image.second._fileName.empty())
			}  //  for (auto& image : _images)
		}  //  if (!_pOptions->_simulate)

		//  write Links to file
		if (!_pOptions->_linkFileNameWrite.empty()) {
			ofstream	outFile(_pOptions->_linkFileNameWrite);

			for (auto& entry : _links) {
				outFile << "L," << entry.second << endl;
			}
			for (auto& entry : _images) {
				outFile << "I," << entry.second << endl;
			}

			outFile.flush();
			outFile.close();
		}
	} else {
		UrlLink		newLink;
		ifstream	inFile(_pOptions->_linkFileNameRead);
		string		type;

		fprintf(stderr, "  reading links from %s\n", _pOptions->_linkFileNameRead.c_str());

		//  read file line by line
		while (getline(inFile, type, ',')) {
			inFile >> newLink;

			if (type == "L") {
				_links[newLink._link] = newLink;
			}
			else if (type == "I") {
				_images[newLink._link] = newLink;
			}
		}
		inFile.close();
	}

	//  show links if set
	if (_pOptions->_showLinks) {
		fprintf(stderr, "\nfound links: (%d)\n", _links.size());
		for (auto& link : _links) {
			fprintf(stderr, "  %02d - %s\n", link.second._depth, link.first.c_str());
		}
		fprintf(stderr, "\n");
	}

	//  show images if set
	if (_pOptions->_showImages) {
		fprintf(stderr, "\nfound images: (%d)\n", _images.size());
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
