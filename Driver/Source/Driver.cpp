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

#ifdef _WIN32
#include <Windows.h>
#endif

#include <fstream>

#include "WaveCompiler/Parser/Parser.h"

#include "ArgParse.h"
#include "DiagnosticReporter.h"

using namespace Wave;

int main(int argc, char** argv)
{
	// Get colors working on Windows
#ifdef _WIN32

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (out == INVALID_HANDLE_VALUE)
	{
		return 1;
	}

	DWORD outMode = 0;
	if (!GetConsoleMode(out, &outMode))
	{
		return 1;
	}

	outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(out, outMode))
	{
		return 1;
	}

#endif

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

		Parser parser(Context, lexer);
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
