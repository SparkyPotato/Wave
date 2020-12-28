// Copyright 2021 SparkyPotato

#include "WaveCompiler/CompileContext.h"

#include "ArgParse.h"
#include "DiagnosticReporter.h"

using namespace Wave;

int main(int argc, char** argv)
{
	ParseArguments(argc, argv);

	auto context = CompileContext();

	return 0;
}
