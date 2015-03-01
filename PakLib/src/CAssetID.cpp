#include "CAssetID.hpp"
#include <memory.h>

const CAssetID CAssetID::InvalidAsset;

CAssetID::CAssetID()
    : m_idLen(E_Invalid)
{
    memset(m_idVal, 0xFF, 16);
}

CAssetID::CAssetID(Athena::io::IStreamReader& input, CAssetID::EIDBits idLength)
    : m_idLen(idLength)
{
    if (idLength != E_Invalid)
    {
        memset(m_idVal, 0, 16);
        atUint32 bytes = this->idLength();
        atUint8* tmp = input.readUBytes(bytes);
        memcpy(m_idVal, tmp, bytes);
        delete[] tmp;
    }
    else
        memset(m_idVal, 0xFF, 16);
}

CAssetID::CAssetID(const atUint8* assetID, CAssetID::EIDBits idLength)
    : m_idLen(idLength)
{
    if (idLength != E_Invalid)
    {
        atUint32 bytes = this->idLength();
        memset(m_idVal, 0, 16);
        memcpy(m_idVal, assetID, bytes);
    }
    else
        memset(m_idVal, 0xFF, 16);
}

CAssetID::CAssetID(const atUint32& assetID)
{
    fromUint32(assetID);
}

CAssetID::CAssetID(const atUint64& assetID)
{
    fromUint64(assetID);
}

CAssetID::~CAssetID()
{
}

atUint32 CAssetID::toUint32() const
{
    atUint32 id = *(atUint32*)(m_idVal);
    Athena::utility::BigUint32(id);

    return id;
}

atUint64 CAssetID::toUint64() const
{
    atUint64 id = *(atUint64*)(m_idVal);
    Athena::utility::BigUint64(id);

    return id;
}

atUint32 CAssetID::idLength() const
{
    switch(m_idLen)
    {
        case E_32Bits: return 4;
        case E_64Bits: return 8;
        case E_128Bits: return 16;
        default: return 0;
    }

}

atUint8* CAssetID::raw() const
{
    return (atUint8*)m_idVal;
}

void CAssetID::fromUint32(const atUint32& other)
{
    atUint32 tmp = other;
    Athena::utility::BigUint32(tmp);
    memset(m_idVal, 0, 16);
    memcpy(m_idVal, &tmp, 4);
    m_idLen = E_32Bits;
}

CAssetID& CAssetID::operator=(atUint32 other)
{
    fromUint32(other);
    return *this;
}

void CAssetID::fromUint64(const atUint64& other)
{
    atUint64 tmp = other;
    Athena::utility::BigUint64(tmp);
    memset(m_idVal, 0, 16);
    memcpy(m_idVal, &tmp, 8);
    m_idLen = E_64Bits;
}

CAssetID& CAssetID::operator=(atUint64 other)
{
    fromUint64(other);
    return *this;
}


std::string CAssetID::toString() const
{
    std::string ret = "";
    atUint32 idLen = idLength();
    for (atUint32 i = 0; i < idLen; i++)
        ret += Athena::utility::sprintf("%2.2x", m_idVal[i]);
    return ret;
}

bool CAssetID::operator==(atUint32 right) const
{
    return (toUint32() == right);
}

bool CAssetID::operator!=(atUint32 right) const
{
    return (toUint32() != right);
}

bool CAssetID::operator==(atUint64 right) const
{
    return (toUint64() == right);
}

bool CAssetID::operator!=(atUint64 right) const
{
    return (toUint64() != right);
}

bool CAssetID::operator==(const CAssetID& right) const
{
    return !memcmp(m_idVal, right.m_idVal, 16);
}

bool CAssetID::operator!=(const CAssetID& right) const
{
    return memcmp(m_idVal, right.m_idVal, 16) != 0;
}
