import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

# Colors
CRED = "\033[91m"
CGREEN = "\033[92m"
CCYAN = "\033[96m"
CEND = "\033[0m"

def GenerateFiles(args):
    # Create build directory in root folder
    buildDir = os.path.realpath(os.path.dirname(__file__)) + "/Build/" + args.config
    try:
        Path(buildDir).mkdir(parents=True, exist_ok=True)
    except OSError:
        os.chdir(buildDir)
        return True
        
    os.chdir(buildDir)
    
    buildOptions = {
        "Debug"       : "-DCMAKE_BUILD_TYPE=Debug",
        "Development" : "-DCMAKE_BUILD_TYPE=RelWithDebInfo",
        "Release"     : "-DCMAKE_BUILD_TYPE=Release",
    }
    
    options = buildOptions.get(args.config, "");
    # Turn submodule updating off
    if args.nosub:
        options += " -DSUBMODULE=OFF "
    
    # Generate CMake files
    value = subprocess.call(
        "cmake -G Ninja " + options + " ../..", 
        stdout=subprocess.DEVNULL,
        stderr=sys.stderr
    )
    if value != 0:
        print(CRED + "Failed to generate files!" + CEND)
        os.chdir("../..")
        shutil.rmtree(buildDir)
        return False
        
    print(CGREEN + "Generated files." + CEND)
    return True

def Build():
    value = subprocess.call(
        "cmake --build .", 
        stdout=subprocess.DEVNULL,
        stderr=sys.stderr
    )
    
    if value != 0:
        print(CRED + "Build failed!" + CEND)
        return
        
    print(CGREEN + "Built." + CEND)
        
def Main():
    os.system("color")

    parser = argparse.ArgumentParser(
        description="Generate Ninja build files for Wave, and build the compiler.",
        epilog="If you wish to invoke CMake directly, go to the README."
    )
    
    parser.add_argument(
        "-c", "--conf",
        default="Release",
        choices=["Debug", "Development", "Release"],
        help="Configuration to build, directly corresponds to Debug, RelWithDebInfo, and Release.",
        dest="config"
    )
    
    parser.add_argument(
        "-nosub", "--nosubmodule",
        action="store_false",
        help="Disable updating submodules, only turn off if you have made changes to them.",
        dest="nosub"
    )
    
    args = parser.parse_args()
    
    if GenerateFiles(args):
        Build()

if __name__ == "__main__":
    Main()
    