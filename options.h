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
		virtual	bool			parseNameFile(map<string, string>& mapTarget, string const fileName);

	public:
		map<string, string>		_includes;
		map<string, string>		_excludes;
		map<string, string>		_imageNames;
		string					_linkFileNameRead;
		string					_linkFileNameWrite;
		unsigned char			_recurseDepth = 0;
		bool					_showLinks    = false;
		bool					_showImages   = false;
		bool					_simulate     = false;
		bool					_parseHeader  = false;
		bool					_includeCss   = false;
		bool					_includeJs   = false;

		virtual					~Options();
		static	Options*		getInstance();

		virtual	bool			parse(int argc, char** argv);
};
#endif  //  OPTIONS_H
