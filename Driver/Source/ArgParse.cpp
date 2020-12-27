#include "ArgParse.h"

namespace Wave {

namespace Args {

std::vector<fs::path> SourceFiles;

}

void ParseArguments(int argc, const char* const* argv)
{
	for (int i = 0; i < argc; i++)
	{
		// If the first character of the argument string is not a hyphen '-',
		// it is a source file.
		if (argv[i][0] != '-')
		{
			Args::SourceFiles.emplace_back(argv[i]);
		}
	}
}

}
