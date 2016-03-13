#include "options.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>

//-----------------------------------------------------------------------------
Options*	Options::_pInstance = nullptr;

//-----------------------------------------------------------------------------
Options::Options()
{}

//-----------------------------------------------------------------------------
Options::~Options()
{}

//-----------------------------------------------------------------------------
Options* Options::getInstance()
{
	if (_pInstance == nullptr) {
		_pInstance = new Options();
	}
	return _pInstance;
}

//-----------------------------------------------------------------------------
bool Options::parse(int argc, char** argv)
{
	int		opt           (0);

	//  decode options
	while ((opt = getopt(argc, argv, "d:D:e:E:hi:I:l:r:s")) != -1) {
		switch (opt) {
			case 'd':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify an image/video name!\033[0m\n");
					return usage();
				}
				_imageNames[optarg] = optarg;
				break;
			case 'D':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify a image/video name file!\033[0m\n");
					return usage();
				}
				parseNameFile(_imageNames, optarg);
				break;
			case 'e':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify a link text!\033[0m\n");
					return usage();
				}
				_excludes[optarg] = optarg;
				break;
			case 'E':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify a link text file!\033[0m\n");
					return usage();
				}
				parseNameFile(_excludes, optarg);
				break;
			case 'h':
				_parseHeader = true;
				break;
			case 'i':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify a link text!\033[0m\n");
					return usage();
				}
				_includes[optarg] = optarg;
				break;
			case 'I':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify a link text file!\033[0m\n");
					return usage();
				}
				parseNameFile(_includes, optarg);
				break;
			case 'l':
				if (*optarg == 'l') {
					_showLinks = true;
				} else if (*optarg == 'i') {
					_showImages = true;
				} else {
					return usage();
				}
				break;
			case 'r':
				_recurseDepth = atoi(optarg);
				break;
			case 's':
				_simulate = true;
				break;
			default:
				return usage();
		}  //  switch (opt)
	}  //  while ((opt = getopt(argc, argv, ...

	//  check for arguments
	if ((argc - optind) != 2) {
		return usage();
	}

	return true;
}

//-----------------------------------------------------------------------------
bool Options::parseNameFile(map<string,string>& mapTarget, string const fileName)
{
	FILE*	pFile(fopen(fileName.c_str(), "rb"));

	if (pFile != nullptr) {
		char	cBuffer[10000] = {0};

		while (fgets(cBuffer, 10000, pFile) != nullptr) {
			cBuffer[strlen(cBuffer) - 1] = 0;
			mapTarget[cBuffer] = cBuffer;
		}

		fclose(pFile);
	}

	return true;
}

//-----------------------------------------------------------------------------
bool Options::usage()
{
	printf("\nUsage: offshore [options] TARGET URL\n"
			"Download URL to local file.\n\n"
			"  -d TEXT\tdownload images/movies containing TEXT in path (multiple occurancy possible)\n"
			"  -D FILE\tdownload images/movies containing texts defined in FILE\n"
			"  -e TEXT\texclude links containing TEXT (multiple occurancy possible)\n"
			"  -E FILE\texclude links containing texts defined in FILE\n"
			"  -h\t\tconsider links in page header (default: false)\n"
			"  -i TEXT\tinclude links containing TEXT (multiple occurancy possible)\n"
			"  -I FILE\tinclude links containing texts defined in FILE\n"
			"  -li\t\tshow all referenced images\n"
			"  -ll\t\tshow all referenced links\n"
			"  -r DEPTH\trecursive follow links upto given depth\n"
			"  -s\t\tsimulate download\n"
			"\n"
		);

	return false;
}