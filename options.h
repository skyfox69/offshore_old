#ifndef OPTIONS_H
#define	OPTIONS_H

//-----------------------------------------------------------------------------
class Options
{
	private:
		static	Options*	_pInstance;

	protected:
							Options();
		virtual	bool		usage();

	public:
		unsigned char		_recurseDepth = 0;
		bool				_showLinks    = false;
		bool				_simulate     = false;

		virtual				~Options();
		static	Options*	getInstance();

		virtual	bool		parse(int argc, char** argv);
};
#endif  //  OPTIONS_H