#ifndef PAKFILEREADER_HPP
#define PAKFILEREADER_HPP

#include <Athena/FileReader.hpp>

struct SectionEntry;
class PakFile;
class PakFileReader final : public Athena::io::FileReader
{
    FILEREADER_BASE();
public:

    PakFileReader(const std::string& filename);
    virtual ~PakFileReader();

    PakFile* read();

    static PakFile* load(const std::string& filename);
    void loadResourceTable(PakFile* ret);
    void loadNameTable(PakFile* ret);
    std::vector<SectionEntry> loadSectionTable();
};

#endif // PAKFILEREADER_HPP
