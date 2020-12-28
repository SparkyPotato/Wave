// Copyright 2021 SparkyPotato

#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace Wave {

/// Severity of diagnostic output
enum class Severity
{
	Note, 
	Warning, 
	Error, 
	Fatal // Only difference between error and fatal is that fatal will instantly exit.
};

/// Diagnostic reporter for the Wave compiler driver.
class DiagnosticReporter
{
public:
	/// Construct a diagnostic reporter for output.
	///
	/// \param location The location of the diagnostic,
	/// if it did not come from a file, use 'wavec'.
	/// \param severity The severity of the diagnostic message.
	DiagnosticReporter(const std::string& location, Severity severity);

	/// Append to the output message.
	///
	/// \tparam T Any type supported by std::ostream.
	/// \param val The value to append.
	/// 
	/// \return The DiagnosticReporter object, so you can chain the stream output operator.
	template<typename T>
	DiagnosticReporter& operator<<(const T& val)
	{
		m_Buf << val;
		return *this;
	}

	/// Dump the message to the console.
	/// Exits with error code 1 if severity was set to Severity::Fatal.
	void Dump();

private:
	std::ostringstream m_Buf;
	Severity m_Severity;
};

}
