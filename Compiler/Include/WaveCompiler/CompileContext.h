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

#include "Global.h"

namespace Wave {

/// Class for storage of compiler options,
/// currently does nothing.
class WAVEC_API CompileContext
{
public:
	CompileContext() = default;

	CompileContext(const CompileContext&) = delete;
	CompileContext& operator=(const CompileContext&) = delete;

	/// Set compile debug output.
	/// Output will be printed to stdout.
	void SetDebugOutput(bool on = false);

	/// Check if debug output is enabled.
	///
	/// \return If debug ouput is enabled.
	bool IsDebugOutputEnabled() { return m_DebugOutput; }

private:
	bool m_DebugOutput = false;
};

}
