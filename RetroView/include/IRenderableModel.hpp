#ifndef IRENDERABLE_HPP
#define IRENDERABLE_HPP

#include <string>
#include <SBoundingBox.hpp>
#include <Athena/Types.hpp>

class IRenderableModel
{
public:
    IRenderableModel(atUint64 assetId)
        : m_assetId(assetId)
    {
    }

    atUint64 assetId() { return m_assetId; }
    virtual ~IRenderableModel() {}
    virtual void draw()       =0;
    virtual void drawBoundingBox() =0;
    virtual void exportToObj(const std::string&) {}
    virtual bool canExport() const {return false;}
    virtual void invalidate() {}
    virtual void rebuildShaders() {}
    virtual SBoundingBox& boundingBox()=0;
protected:
    atUint64 m_assetId;
};

#endif // IRENDERABLE_HPP
