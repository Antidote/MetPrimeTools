#include <iostream>
#include "PakFileReader.hpp"
#include "PakFile.hpp"

#ifndef _WIN32
#include <sys/stat.h>
#define mkdir(x) mkdir(x, 0755);
#else
#include <direct.h> //mkdir()
#endif


int main(int argc, char* argv[])
{
    std::string progName = argv[0];
    progName = progName.substr(progName.find_last_of("/\\") + 1);
    if (argc < 2)
    {
        printf("Usage: %s <in> [out]\n", progName.c_str());
        return 1;
    }

    std::string inName = argv[1];
    std::string outName;
    if (argc >= 3)
        outName = argv[2];

    if (outName == std::string())
    {
        outName = inName.substr(0, inName.rfind('.'));
        if (outName == std::string())
            outName = inName;
    }

    PakFile* pak = nullptr;
    try
    {
        std::cout << "Extracting pack to: " << outName << std::endl;
        PakFileReader reader(inName);
        pak = reader.read();
        mkdir(outName.c_str());
        pak->dumpPak(outName);
        std::cout << "\ndone" << std::endl;
    }
    catch(const Athena::error::Exception& e)
    {
        std::cout << e.message() << std::endl;
        delete pak;
        pak = nullptr;
    }

    return 0;
}
