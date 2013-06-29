#include "asterror.h"
#include "parser.h"

namespace parsing
{
	ASTError::ASTError(Token s, Expectation &e, string m)
	{
		src = s;
		exp = &e;
		msg = m;
	}

	string ASTError::format(string fmt)
	{
		string tmp = fmt;
		if (tmp.find("%l") != string::npos)
		{
			stringstream ss;
			ss << src.getLine();
			tmp.replace(tmp.find("%l"), 2, ss.str());
		}

		if (tmp.find("%c") != string::npos)
		{
			stringstream ss;
			ss << src.getColumn();
			tmp.replace(tmp.find("%c"), 2, ss.str());
		}

		if (tmp.find("%f") != string::npos)
		{
			tmp.replace(tmp.find("%f"), 2, src.getFile());
		}

		if (tmp.find("%t") != string::npos)
		{
			tmp.replace(tmp.find("%t"), 2, src.getType());
		}

		if (tmp.find("%s") != string::npos)
		{
			tmp.replace(tmp.find("%s"), 2, src.get());
		}

		if (tmp.find("%C") != string::npos)
		{
			tmp.replace(tmp.find("%C"), 2, exp->cls);
		}

		if (tmp.find("%n") != string::npos)
		{
			tmp.replace(tmp.find("%n"), 2, exp->name);
		}

		if (tmp.find("%e") != string::npos)
		{
			tmp.replace(tmp.find("%e"), 2, exp->expecting);
		}

		if (tmp.find("%m") != string::npos)
		{
			tmp.replace(tmp.find("%m"), 2, format(msg));
		}

		return tmp;
	}

	string ASTError::display(string fmt)
	{
		return format(fmt);
	}

	string ASTError::display()
	{
		if (!(exp))
			return display("\033[1;28mError \033[0;0m- %f:\033[1;28m%l\033[0;0m:%c - \033[0;31m%m\033[0;0m\n");
		else
			return display("\033[1;28mError \033[0;0m- %f:\033[1;28m%l\033[0;0m:%c - \033[0;31m%C %n expected %e, but got %t '%s'\033[0;0m\n");
	}
}