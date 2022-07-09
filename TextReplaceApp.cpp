//MIT License
//
//Copyright(c) 2022 Jashen Low
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this softwareand associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#if (!defined(_WIN32) && !defined(_WIN64)) || defined(__linux__) || defined(__APPLE__)
#include <cstring>
#include <algorithm>
#endif

#define COMMENT_CHAR '#'
#define REPLACE_LIST_DELIM '='
#define ARG_HELP    "-h"
#define ARG_INFILE  "-i"
#define ARG_OUTFILE "-o"

typedef std::vector<std::pair<std::string, std::string>> WordReplaceList;    //[TO_REPLACE, REPLACED_WITH]

bool ReadReplaceList(const char* filename, WordReplaceList& wordList)
{
    std::fstream file;

    file.open(filename, std::ios::in);

    if (file.is_open())
    {
        std::string lineText;

        printf("Reading \"%s\"...\n", filename);

        while (std::getline(file, lineText))        //Read each line in the file
        {
            if (lineText.front() != COMMENT_CHAR)   //Skip lines that are intended to be comments
            {
                std::string::size_type delimPos = lineText.find(REPLACE_LIST_DELIM);

                if (delimPos != std::string::npos)  //Skip lines that don't contain the delimiter character.
                {
                    lineText.erase(std::remove(lineText.begin(), lineText.end(), ' '), lineText.end());         //Remove any spaces in the line of text.

                    if ((lineText.front() != REPLACE_LIST_DELIM) && (lineText.back() != REPLACE_LIST_DELIM))   //Skip line if the first or last character is the delimiter.
                    {
                        std::pair<std::string, std::string> wordPair;

                        wordPair.first  = std::move(lineText.substr(0, delimPos - 1));
                        wordPair.second = std::move(lineText.substr(delimPos, lineText.length() - delimPos));
                        wordList.emplace_back(wordPair);
                    }
                }                
            }
        }
        
        printf("\tWords to replace: %zd.\n", wordList.size());
        
        if (wordList.empty())
            printf("\tInput text file will not be processed, since no replacement words were found!\n");
        
        file.close();
        return true;
    }
    else  
        return false;
}

void PrintHelpMenu()
{
    std::cout << "Example Usage: -i [INPUT_PATH] -o [OUTPUT_PATH]" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------- COMMANDS -----------------" << std::endl;
    std::cout << "\t-h" << "\tPrints this menu." << std::endl;
    std::cout << "\t-i" << "\tSpecifies input file path." << std::endl;
    std::cout << "\t-o" << "\tSpecifies output file path." << std::endl;
}

bool ReadCmdArgs(int argc, char** argv, std::string& inFile, std::string& outFile)
{
    if (argc > 1)
    {
        std::string argStr;

        if (strcmp(argv[1], ARG_HELP) == 0)
            PrintHelpMenu();
        else
        {
            for (int i = 1; i < argc; i++)
            {
                if (argv[i] != nullptr)
                {
                    if (strcmp(argv[i], ARG_INFILE) == 0)   //Store input file path.
                    {
                        char* arg = argv[i + 1];
                        if (arg != nullptr)
                        {
                            argStr = arg;

                            if ((argStr != ARG_HELP) && (argStr != ARG_INFILE) && (argStr != ARG_OUTFILE))
                                inFile = argStr;
                        }
                    }
                    else if (strcmp(argv[i], ARG_OUTFILE) == 0)   //Store output file path.
                    {
                        char* arg = argv[i + 1];
                        if (arg != nullptr)
                        {
                            argStr = arg;

                            if ((argStr != ARG_HELP) && (argStr != ARG_INFILE) && (argStr != ARG_OUTFILE))
                                outFile = argStr;
                        }
                    }
                }
            }

            if (inFile.empty() || outFile.empty())
                return false;
        }

        return true;
    }
    else
        return false;
}

int main(int argc, char** argv)
{
    constexpr const char* replaceFileListName = "replace_list.cfg";

    WordReplaceList wordList;                     //Stores mapping of all words to be replaced.
    std::vector<std::string> storedFileText;    //Stores a copy of each line in the input text file. It is also used to write to the output text file.
    std::string inFilePath, outFilePath;

    if (!ReadCmdArgs(argc, argv, inFilePath, outFilePath))
    {
        if (inFilePath.empty() && outFilePath.empty())
            printf("ERROR: No arguments were entered Exiting...\n");
        else
        {
            if (inFilePath.empty())
                printf("ERROR: Input file path is empty! Exiting...\n");
            else
            {
                if (outFilePath.empty())
                {
                    printf("WARNING: Output file path is empty! Input file will be overwritten instead. Continue?\n");
                    system("pause");

                    outFilePath = inFilePath;
                }
            }
        }
    }

    if (!inFilePath.empty() && !outFilePath.empty())
    {
        if (ReadReplaceList(replaceFileListName, wordList))
        {
            if (!wordList.empty())
            {
                std::fstream file;

                //Attempt to open the input text file.
                file.open(inFilePath, std::ios::in);

                if (file.is_open())
                {
                    std::string lineText;

                    printf("Reading input file...\n");
                    
                    while (std::getline(file, lineText))    //Load each line into a string vector.
                        storedFileText.emplace_back(lineText);

                    file.close();   //Close input file.

                    if (!storedFileText.empty())
                    {
                        size_t lineReplaceCount = 0;

                        for (auto& line : storedFileText)   //Search each line for text to replace
                        {
                            bool lineEdited = false;

                            for (auto& word : wordList)
                            {
                                std::string::size_type index = 0;

                                while (index != std::string::npos)
                                {
                                    index = line.find(word.first.c_str(), index);

                                    if (index != std::string::npos)
                                    {
                                        line.replace(index, word.first.length(), word.second, 0, word.second.length());
                                        index += word.second.length();

                                        lineEdited = true;
                                    }
                                }
                            }

                            line.append("\n");  //Add a line break since each of the stored lines don't have it.

                            if (lineEdited)
                                lineReplaceCount++;
                        }

                        printf("%zd lines were changed.\n", lineReplaceCount);

                        if (lineReplaceCount > 0)
                        {
                            printf("Writing output file...\n");

                            file.open(outFilePath, std::ios::out);

                            if (file.is_open())
                            {
                                for (auto& line : storedFileText)
                                    file << line;

                                file.close();   //Close output file.
                                printf("Output file written successfully!\n");
                            }
                            else
                                printf("ERROR: Unable to write to file \"%s\".", outFilePath.c_str());
                        }
                        else
                            printf("Output file will not be written, since there are no changes.\n");
                    }
                    else
                        printf("ERROR: Input text file is empty! Nothing to be done here...\n");
                }
                else
                    printf("ERROR: Unable to open text file \"%s\"! Exiting...", inFilePath.c_str());
            }
        }
        else
            printf("ERROR: Unable to open word replacement file \"%s\".\n", replaceFileListName);
    }
}

