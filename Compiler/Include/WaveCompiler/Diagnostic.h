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

#include <filesystem>
#include <fstream>
#include <string>

#include "Global.h"

namespace Wave {

/// A specific position in a specific file.
struct WAVEC_API FileMarker
{
	/// Construct a FileMarker from an std::ifstream.
	///
	/// \param filePath The path of the file.
	FileMarker(const std::filesystem::path& filePath)
		: File(filePath)
	{}

	/// Path of the file the marker points to.
	std::filesystem::path File;

	/// Position of the first character of the marker. 0 is the first character of the stream.
	uint64_t Pos = 0;

	/// Length of the marker, including the first character.
	uint64_t Length = 0;
};

/// Severity of diagnostic.
enum class DiagnosticSeverity
{
	Note,
	Warning,
	Error,
	Fatal // Only difference between error and fatal is that fatal will instantly exit.
};

/// Wave compile-time diagnostic.
struct WAVEC_API Diagnostic
{
	/// Construct a Diagnostic from a file marker.
	///
	/// \param marker The marker to move into the FileMarker.
	/// \param severity The severity of the diagnostic.
	/// \param message The diagnostic message.
	Diagnostic(const FileMarker& marker, DiagnosticSeverity severity, const std::string& message)
		: Marker(marker), Severity(severity), Message(message)
	{}

	/// Marker of the diagnostic position.
	FileMarker Marker;

	/// Severity of the compiler diagnostic.
	DiagnosticSeverity Severity;

	/// Diagnostic message.
	std::string Message;
};

}
