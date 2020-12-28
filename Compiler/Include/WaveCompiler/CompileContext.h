// Copyright 2021 SparkyPotato

#pragma once

namespace Wave {

/// Class for storage of compiler options,
/// currently does nothing.
class CompileContext
{
public:
	CompileContext() = default;

	CompileContext(const CompileContext&) = delete;
	CompileContext& operator=(const CompileContext&) = delete;
};

}
