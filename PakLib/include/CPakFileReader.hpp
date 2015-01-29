#ifndef PAKFILEREADER_HPP
#define PAKFILEREADER_HPP

#include <Athena/FileReader.hpp>

struct SectionEntry;
class CPakFile;
class CPakFileReader final : public Athena::io::FileReader
{
    FILEREADER_BASE();
public:

    CPakFileReader(const std::string& filename);
    virtual ~CPakFileReader();

    CPakFile* read();

    static CPakFile* load(const std::string& filename);
    void loadResourceTable(CPakFile* ret);
    void loadNameTable(CPakFile* ret);
    std::vector<SectionEntry> loadSectionTable();
};

#endif // PAKFILEREADER_HPP
