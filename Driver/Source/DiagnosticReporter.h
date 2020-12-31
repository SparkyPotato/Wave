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

#include <iostream>
#include <sstream>
#include <string>

#include "WaveCompiler/Diagnostic.h"

namespace Wave {

/// Diagnostic reporter for the Wave compiler driver.
class DiagnosticReporter
{
public:
	/// Construct a diagnostic reporter for output.
	///
	/// \param location The location of the diagnostic,
	/// if it did not come from a file, use 'wavec'.
	/// \param severity The severity of the diagnostic message.
	DiagnosticReporter(const std::string& location, DiagnosticSeverity severity);

	/// Construct a diagnostic reporter from a compiler diagnostic.
	/// Does not check for validity of the diagnostic.
	///
	/// \param diagnostic The diagnostic object to report.
	DiagnosticReporter(const Diagnostic& diagnostic);

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
	DiagnosticSeverity m_Severity;
};

}
