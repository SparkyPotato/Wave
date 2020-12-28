// Copyright 2021 SparkyPotato

#include "DiagnosticReporter.h"

namespace Wave {

namespace {

constexpr const char* EscapeEnd = "\033[0m";
constexpr const char* EscapeRed = "\033[91m";
constexpr const char* EscapeGreen = "\033[92m";
constexpr const char* EscapeYellow = "\033[93m";

}

DiagnosticReporter::DiagnosticReporter(const std::string& location, Severity severity)
{
	m_Buf << location << ": ";
	switch (severity)
	{
	case Severity::Note: m_Buf << "note: "; break;
	case Severity::Warning: m_Buf << EscapeYellow << "warning: " << EscapeEnd; break;
	case Severity::Error:
	case Severity::Fatal: m_Buf << EscapeRed << "error: " << EscapeEnd; break;
	}
}

void DiagnosticReporter::Dump()
{
	if (m_Severity != Severity::Note) { std::cerr << m_Buf.str() << "\n"; }
	else { std::cout << m_Buf.str() << "\n"; }

	if (m_Severity == Severity::Fatal) { exit(1); }
}

}
