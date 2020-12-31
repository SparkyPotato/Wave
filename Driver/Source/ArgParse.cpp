// Copyright 2021 SparkyPotato
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
				DiagnosticReporter diag("wavec", DiagnosticSeverity::Fatal);
				diag << "source file does not exist: '" << path.string() << "'.";
				diag.Dump();
			}

			Args::SourceFiles.emplace_back(std::move(path));
		}
		else // No command line options are supported yet.
		{
			DiagnosticReporter diag("wavec", DiagnosticSeverity::Warning);
			diag << "ignoring unknown option: '" << argv[i] << "'.";
			diag.Dump();
		}
	}
}

}
