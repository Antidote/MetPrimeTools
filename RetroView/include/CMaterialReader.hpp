#ifndef MATERIALREADER_HPP
#define MATERIALREADER_HPP


#include <Athena/MemoryReader.hpp>
#include "CMaterialSet.hpp"

enum class EMaterialCommand
{
    BLOD = 0x424C4F44,
    BLOI = 0x424C4F49,
    BLOL = 0x424C4F4C,
    BNIF = 0x424E4946,
    CLR  = 0x434C5220,
    DIFF = 0x44494646,
    DIFB = 0x44494642,
    END  = 0x454E4420,
    INCA = 0x494E4341,
    INT  = 0x494E5420,
    LURD = 0x4C555244,
    LRLD = 0x4C524C44,
    PASS = 0x50415353,
    RFLD = 0x52464c44,
    RFLV = 0x52464c56,
    RIML = 0x52494D4C,
    TRAN = 0x5452414E,
    XRAY = 0x58524159
};


class CMaterialReader final : protected Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
public:
    CMaterialReader(const atUint8* data, atUint64 length);
    CMaterialReader(const std::string&) = delete;
    ~CMaterialReader();

    CMaterialSet read(CMaterial::Version version);
private:
    bool readMaterialCommand(CMaterialSet& set, CMaterial& material);
};

#endif // MATERIALREADER_HPP
