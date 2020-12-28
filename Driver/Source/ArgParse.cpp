// Copyright 2021 SparkyPotato

#include "ArgParse.h"

#include "DiagnosticReporter.h"

namespace Wave {

namespace Args {

std::vector<fs::path> SourceFiles;

}

void ParseArguments(int argc, const char* const* argv)
{
	for (int i = 1; i < argc; i++)
	{
		// If the first character of the argument string is not a hyphen '-',
		// it is a source file.
		if (argv[i][0] != '-')
		{
			fs::path path = argv[i];
			if (!fs::exists(path) || !fs::is_regular_file(path))
			{
				DiagnosticReporter diag("wavec", Severity::Fatal);
				diag << "source file does not exist: '" << path.string() << "'.";
				diag.Dump();
			}

			Args::SourceFiles.emplace_back(std::move(path));
		}
		else // No command line options are supported yet.
		{
			DiagnosticReporter diag("wavec", Severity::Warning);
			diag << "ignoring unknown option: '" << argv[i] << "'.";
			diag.Dump();
		}
	}
}

}
