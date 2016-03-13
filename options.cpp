#include "options.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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
	while ((opt = getopt(argc, argv, "d:e:i:lr:s")) != -1) {
		switch (opt) {
			case 'd':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify an image/video name!\033[0m\n");
					return usage();
				}
				_imageNames[optarg] = optarg;
				break;
			case 'e':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify a link text!\033[0m\n");
					return usage();
				}
				_excludes[optarg] = optarg;
				break;
			case 'i':
				if (*optarg == 0) {
					printf("\x1B[31mPlease specify a link text!\033[0m\n");
					return usage();
				}
				_includes[optarg] = optarg;
				break;
			case 'l':
				_showLinks = true;
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
bool Options::usage()
{
	printf("\nUsage: offshore [options] TARGET URL\n"
			"Download URL to local file.\n\n"
			"  -d TEXT\tdownload images/movies containing TEXT in path (multiple occurancy possible)\n"
			"  -e TEXT\texclude links containing TEXT (multiple occurancy possible)\n"
			"  -i TEXT\tinclude links containing TEXT (multiple occurancy possible)\n"
			"  -l\t\tshow all referenced links\n"
			"  -r DEPTH\trecursive follow links upto given depth\n"
			"  -s\t\tsimulate download\n"
			"\n"
		);

	return false;
}