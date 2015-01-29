#ifndef MATERIALREADER_HPP
#define MATERIALREADER_HPP


#include <Athena/BinaryReader.hpp>
#include "CMaterialSet.hpp"

class CMaterialReader final : protected Athena::io::BinaryReader
{
    BINARYREADER_BASE();
public:
    CMaterialReader(const atUint8* data, atUint64 length);
    CMaterialReader(const std::string&) = delete;
    ~CMaterialReader();

    CMaterialSet read(CMaterial::Version version);
};

#endif // MATERIALREADER_HPP
