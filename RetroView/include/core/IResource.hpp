#ifndef IRESOURCE
#define IRESOURCE

#include <string>
#include <Athena/Types.hpp>
#include "CFourCC.hpp"
#include "CPakFile.hpp"
#include <QWidget>

class IResource
{
public:
    virtual ~IResource() {}
    CUniqueID assetId() const { return m_assetID; }
    CFourCC assetType() const { return m_assetType; }

    CPakFile* source() const { return m_source; }
    QWidget* widget() const { return nullptr; }
    void destroy();
protected:
    friend class CResourceManager;
    CUniqueID m_assetID;
    CFourCC  m_assetType;
    CPakFile* m_source;
};

#endif // IRESOURCE

