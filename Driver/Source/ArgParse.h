// Copyright 2021 SparkyPotato

#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <vector>

namespace Wave {

/// Parse arguments and fill them in namespace Args.
///
/// \param argc Main argc argument
/// \param argv Main argv argument, const-ified.
void ParseArguments(int argc, const char* const* argv);

namespace Args {

extern std::vector<fs::path> SourceFiles;

}

}
