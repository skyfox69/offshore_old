#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
#include "options.h"
#include "urlloader.h"
#include "urlcrawler.h"

int main(int argc, char** argv)
{
	//  initialize options
	if (!Options::getInstance()->parse(argc, argv)) {
		delete Options::getInstance();
		return EX_USAGE;
	}

	//  initialize urlloader
	UrlLoader::getInstance();

	//  initialize crawler
	UrlCrawler*		pCrawler(new UrlCrawler());

	//  crawl url
	pCrawler->crawlHtml(argv[optind+1], argv[optind]);

	//  clean up
	delete pCrawler;
	delete UrlLoader::getInstance();
	delete Options::getInstance();

	return EX_OK;
}

