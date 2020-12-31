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

#include "DiagnosticReporter.h"

namespace Wave {

namespace {

constexpr const char* EscapeEnd = "\033[0m";
constexpr const char* EscapeRed = "\033[91m";
constexpr const char* EscapeGreen = "\033[92m";
constexpr const char* EscapeYellow = "\033[93m";
constexpr const char* EscapeHighlight = "\033[42m";

}

DiagnosticReporter::DiagnosticReporter(const std::string& location, DiagnosticSeverity severity)
{
	m_Buf << location << ": ";

	switch (severity)
	{
	case DiagnosticSeverity::Note: m_Buf << "note: "; break;
	case DiagnosticSeverity::Warning: m_Buf << EscapeYellow << "warning: " << EscapeEnd; break;
	case DiagnosticSeverity::Error:
	case DiagnosticSeverity::Fatal: m_Buf << EscapeRed << "error: " << EscapeEnd; break;
	}
}

DiagnosticReporter::DiagnosticReporter(const Diagnostic& diagnostic)
{
	// <filename>:<line>:<column>: 
	m_Buf << diagnostic.Marker.File.filename().string() << ":";
	m_Buf << diagnostic.Marker.Line << ":";
	m_Buf << diagnostic.Marker.Column << ": ";

	// <severity>:
	switch (diagnostic.Severity)
	{
	case DiagnosticSeverity::Note: m_Buf << "note: "; break;
	case DiagnosticSeverity::Warning: m_Buf << EscapeYellow << "warning: " << EscapeEnd; break;
	case DiagnosticSeverity::Error:
	case DiagnosticSeverity::Fatal: m_Buf << EscapeRed << "error: " << EscapeEnd; break;
	}

	// <message>
	m_Buf << diagnostic.Message << "\n";

	// Offending line, with offending part highlighted.
	std::ifstream stream(diagnostic.Marker.File);
	for (uint64_t i = 0; i < diagnostic.Marker.Line - 1; i++) 
	{
		stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	// Characters before highlight
	for (uint64_t i = 0; i < diagnostic.Marker.Column - 1; i++)
	{
		char c;
		stream.get(c);
		m_Buf << c;
	}
	// Highlight
	m_Buf << EscapeHighlight;
	for (uint64_t i = 0; i < diagnostic.Marker.Length; i++)
	{
		char c;
		stream.get(c);
		m_Buf << c;
	}
	m_Buf << EscapeEnd;
	std::string str;
	std::getline(stream, str);
	m_Buf << str;
}

void DiagnosticReporter::Dump()
{
	if (m_Severity != DiagnosticSeverity::Note) { std::cerr << m_Buf.str() << "\n"; }
	else { std::cout << m_Buf.str() << "\n"; }

	if (m_Severity == DiagnosticSeverity::Fatal) { exit(1); }
}

}
