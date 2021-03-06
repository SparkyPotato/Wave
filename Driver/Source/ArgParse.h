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
#include <vector>

#include "WaveCompiler/CompileContext.h"

namespace fs = std::filesystem;

namespace Wave {

/// Parse arguments and fill them in namespace Args.
///
/// \param argc Main argc argument
/// \param argv Main argv argument, const-ified.
void ParseArguments(int argc, const char* const* argv);

namespace Args {

/// List of all source file paths.
extern std::vector<fs::path> SourceFiles;

}

extern CompileContext Context;

}
