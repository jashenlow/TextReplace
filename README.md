# TextReplace
A simple C++ app which finds and replaces text in a file based on a user-defined wordlist.
## Information
- C++11 or above is required.
- Uses C++ Standard Library only.
- No other third party APIs required.
- Uses std::fstream for reading and writing, so any ASCII readable file would work.
## Compiling
### Windows (MSVC)
Simply create your own Visual Studio project, or compile the source directly using the MSVC compiler via command line.
> **Command Line Example:** cl /O2 /EHsc TextReplaceApp.cpp
### Linux/Mac
The source file can be compiled easily using gcc.
> **Command Line Example:** g++ -std=c++11 -o TextReplaceApp TextReplaceApp.cpp
## Usage
> Example Usage: [APPNAME] -i [INPUT_PATH] -o [OUTPUT_PATH]  
> -h Prints the help menu.  
> -i Specifies input file path.     
> -o Specifies output file path.  
> **Note:** Output file path argument can be omitted, but the program will overwrite the input file instead.
