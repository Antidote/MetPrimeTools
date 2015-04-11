#include "CUniqueID.hpp"
#include <memory.h>

const CUniqueID CUniqueID::InvalidAsset;

CUniqueID::CUniqueID()
    : m_idLen(E_Invalid)
{
    memset(m_idVal, 0xFF, 16);
}

CUniqueID::CUniqueID(Athena::io::IStreamReader& input, CUniqueID::EIDBits idLength)
    : m_idLen(idLength)
{
    atUint32 bytes = this->idLength();
    if (idLength != E_Invalid && bytes > 0)
    {
        memset(m_idVal, 0, 16);
        atUint8* tmp = input.readUBytes(bytes);
        memcpy(m_idVal, tmp, bytes);
        delete[] tmp;
    }
    else
    {
        m_idLen = E_Invalid; // just to make sure
        memset(m_idVal, 0xFF, 16);
    }
}

CUniqueID::CUniqueID(const atUint8* assetID, CUniqueID::EIDBits idLength)
    : m_idLen(idLength)
{
    atUint32 bytes = this->idLength();
    if (idLength != E_Invalid && bytes > 0)
    {
        memset(m_idVal, 0, 16);
        memcpy(m_idVal, assetID, bytes);
    }
    else
    {
        m_idLen = E_Invalid; // just to make sure
        memset(m_idVal, 0xFF, 16);
    }
}

CUniqueID::CUniqueID(const atUint32& assetID)
{
    fromUint32(assetID);
}

CUniqueID::CUniqueID(const atUint64& assetID)
{
    fromUint64(assetID);
}

CUniqueID::~CUniqueID()
{
}

atUint32 CUniqueID::toUint32() const
{
    atUint32 id = *(atUint32*)(m_idVal);
    Athena::utility::BigUint32(id);

    return id;
}

atUint64 CUniqueID::toUint64() const
{
    atUint64 id = *(atUint64*)(m_idVal);
    Athena::utility::BigUint64(id);

    return id;
}

atUint32 CUniqueID::idLength() const
{
    switch(m_idLen)
    {
        case E_32Bits: return 4;
        case E_64Bits: return 8;
        case E_128Bits: return 16;
        default: return 0;
    }

}

atUint8* CUniqueID::raw() const
{
    return (atUint8*)m_idVal;
}

void CUniqueID::fromUint32(const atUint32& other)
{
    atUint32 tmp = other;
    Athena::utility::BigUint32(tmp);
    memset(m_idVal, 0, 16);
    memcpy(m_idVal, &tmp, 4);
    m_idLen = E_32Bits;
}

CUniqueID& CUniqueID::operator=(atUint32 other)
{
    fromUint32(other);
    return *this;
}

void CUniqueID::fromUint64(const atUint64& other)
{
    atUint64 tmp = other;
    Athena::utility::BigUint64(tmp);
    memset(m_idVal, 0, 16);
    memcpy(m_idVal, &tmp, 8);
    m_idLen = E_64Bits;
}

CUniqueID& CUniqueID::operator=(atUint64 other)
{
    fromUint64(other);
    return *this;
}


std::string CUniqueID::toString() const
{
    std::string ret = "";
    atUint32 idLen = idLength();
    for (atUint32 i = 0; i < idLen; i++)
        ret += Athena::utility::sprintf("%2.2x", m_idVal[i]);
    return ret;
}

bool CUniqueID::operator==(atUint32 right) const
{
    return (toUint32() == right);
}

bool CUniqueID::operator!=(atUint32 right) const
{
    return (toUint32() != right);
}

bool CUniqueID::operator==(atUint64 right) const
{
    return (toUint64() == right);
}

bool CUniqueID::operator!=(atUint64 right) const
{
    return (toUint64() != right);
}

bool CUniqueID::operator==(const CUniqueID& right) const
{
    return !memcmp(m_idVal, right.m_idVal, 16);
}

bool CUniqueID::operator!=(const CUniqueID& right) const
{
    return memcmp(m_idVal, right.m_idVal, 16) != 0;
}
