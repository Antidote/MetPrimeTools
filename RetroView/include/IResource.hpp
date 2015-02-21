#ifndef IRESOURCE
#define IRESOURCE

#include <string>
#include <Athena/Types.hpp>
#include "CFourCC.hpp"

class IResource
{
public:
    virtual ~IResource() {}
    atUint64 assetId() const { return m_assetID; }
    CFourCC assetType() const { return m_assetType; }
protected:
    friend class CResourceManager;
    atUint64 m_assetID;
    CFourCC  m_assetType;
};

#endif // IRESOURCE

