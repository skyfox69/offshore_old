#ifndef OPTIONS_H
#define	OPTIONS_H

#include <map>
#include <string>

using namespace std;

//-----------------------------------------------------------------------------
class Options
{
	private:
		static	Options*		_pInstance;

	protected:
								Options();
		virtual	bool			usage();

	public:
		map<string, string>		_includes;
		map<string, string>		_excludes;
		map<string, string>		_imageNames;
		unsigned char			_recurseDepth = 0;
		bool					_showLinks    = false;
		bool					_simulate     = false;

		virtual					~Options();
		static	Options*		getInstance();

		virtual	bool			parse(int argc, char** argv);
};
#endif  //  OPTIONS_H