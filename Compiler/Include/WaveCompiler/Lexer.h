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

#pragma once

#include <istream>
#include <vector>

#include "Global.h"
#include "CompileContext.h"
#include "Diagnostic.h"

namespace Wave {

/// Wave lexer.
class WAVEC_API Lexer
{
public:
	/// Initialize a lexer from an input stream.
	///
	/// \param filePath The path of the file.
	/// \param stream std::istream to read from.
	Lexer(CompileContext& context, const std::filesystem::path& filePath, std::istream& stream);

	/// Run the lexical analyzer.
	void Lex();

	/// Get the diagnostics from lexical analysis.
	///
	/// \return std::vector containing all diagnostics.
	const std::vector<Diagnostic>& GetDiagnostics();

private:
	char GetChar();

	CompileContext& m_Context;
	std::istream& m_Stream;
	std::filesystem::path m_File;
	std::vector<Diagnostic> m_Diagnostics;
};

}
