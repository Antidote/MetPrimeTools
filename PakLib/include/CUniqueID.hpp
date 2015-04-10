#ifndef CUNIQUEID_HPP
#define CUNIQUEID_HPP

#include <Athena/IStreamReader.hpp>

class CUniqueID
{
public:
    static const CUniqueID InvalidAsset;
    enum EIDBits
    {
        E_Invalid,
        E_32Bits,
        E_64Bits,
        E_128Bits
    };

    CUniqueID();
    CUniqueID(Athena::io::IStreamReader& input, EIDBits idLength);
    CUniqueID(const atUint8* assetID, EIDBits idLength);
    CUniqueID(const atUint32& assetID);
    CUniqueID(const atUint64& assetID);

    ~CUniqueID();
    atUint32 toUint32() const;
    atUint64 toUint64() const;
    atUint32 idLength() const;
    atUint8* raw() const;

    std::string toString() const;

    CUniqueID& operator=(atUint32 other);
    CUniqueID& operator=(atUint64 other);

    bool operator==(atUint32 right) const;
    bool operator!=(atUint32 right) const;
    bool operator==(atUint64 right) const;
    bool operator!=(atUint64 right) const;
    bool operator==(const CUniqueID& right)const;
    bool operator!=(const CUniqueID& right)const;
    void fromUint32(const atUint32& other);
    void fromUint64(const atUint64& other);


private:
    EIDBits  m_idLen;
    atUint8  m_idVal[16];
};

#endif // CUNIQUEID_HPP
