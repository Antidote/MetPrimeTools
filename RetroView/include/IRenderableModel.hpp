#ifndef IRENDERABLE_HPP
#define IRENDERABLE_HPP

#include <string>
#include <SBoundingBox.hpp>
#include <Athena/Types.hpp>

class IRenderableModel
{
public:
    virtual ~IRenderableModel() {}
    virtual void draw()       =0;
    virtual void drawBoundingBox() =0;
    virtual void exportToObj(const std::string&) {}
    virtual void updateViewProjectionUniforms(const glm::mat4&, const glm::mat4&) = 0;
    virtual bool canExport() const {return false;}
    virtual void invalidate() {}
    virtual void rebuildShaders() {}
    virtual SBoundingBox& boundingBox()=0;
};

#endif // IRENDERABLE_HPP
