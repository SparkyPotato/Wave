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

#include <fstream>

#include "WaveCompiler/Parser.h"

#include "ArgParse.h"
#include "DiagnosticReporter.h"

using namespace Wave;

int main(int argc, char** argv)
{
	ParseArguments(argc, argv);

	for (auto& file : Args::SourceFiles)
	{
		std::ifstream stream(file);
		Lexer lexer(Context, file, stream);
		lexer.Lex();

		bool error = false;
		for (auto& diag : lexer.GetDiagnostics())
		{
			if (diag.Severity == DiagnosticSeverity::Error || diag.Severity == DiagnosticSeverity::Fatal)
			{
				error = true;
			}

			DiagnosticReporter d(diag);
			d.Dump();
		}
		if (error) { continue; }

		Parser parser(lexer);
		parser.Parse();

		error = false;
		for (auto& diag : parser.GetDiagnostics())
		{
			if (diag.Severity == DiagnosticSeverity::Error || diag.Severity == DiagnosticSeverity::Fatal)
			{
				error = true;
			}

			DiagnosticReporter d(diag);
			d.Dump();
		}
		if (error) { continue; }
	}

	return 0;
}
