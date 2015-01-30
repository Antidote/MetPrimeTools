#ifndef IRESOURCE
#define IRESOURCE

#include <string>
#include <Athena/Types.hpp>

class IResource
{
public:
    virtual ~IResource() {}
    atUint64 assetId() const { return m_assetID; }
protected:
    friend class CResourceManager;
    atUint64 m_assetID;
};

#endif // IRESOURCE

