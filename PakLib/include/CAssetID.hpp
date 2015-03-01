#ifndef CASSETID_HPP
#define CASSETID_HPP

#include <Athena/IStreamReader.hpp>

class CAssetID
{
public:
    enum EIDBits
    {
        E_Invalid,
        E_32Bits,
        E_64Bits,
        E_128Bits
    };

    CAssetID();
    CAssetID(Athena::io::IStreamReader& input, EIDBits idLength);
    CAssetID(const atUint8* assetID, EIDBits idLength);
    CAssetID(const atUint32& assetID);
    CAssetID(const atUint64& assetID);

    ~CAssetID();
    atUint32 toUint32() const;
    atUint64 toUint64() const;
    atUint32 idLength() const;
    atUint8* raw() const;

    std::string toString() const;

    CAssetID& operator=(atUint32 other);
    CAssetID& operator=(atUint64 other);

    bool operator==(atUint32 right) const;
    bool operator!=(atUint32 right) const;
    bool operator==(atUint64 right) const;
    bool operator!=(atUint64 right) const;
    bool operator==(const CAssetID& right)const;
    bool operator!=(const CAssetID& right)const;
    void fromUint32(const atUint32& other);
    void fromUint64(const atUint64& other);


private:
    EIDBits  m_idLen;
    atUint8* m_idVal;
};

#endif // CASSETID_HPP
