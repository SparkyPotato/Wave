#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <vector>

namespace Wave {

void ParseArguments(int argc, const char* const* argv);

namespace Args {

extern std::vector<fs::path> SourceFiles;

}

}
