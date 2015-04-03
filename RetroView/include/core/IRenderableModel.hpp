#ifndef IRENDERABLE_HPP
#define IRENDERABLE_HPP
#include "core/SBoundingBox.hpp"

#include <string>
#include <Athena/Types.hpp>
#include <QColor>

class CMaterialSet;
class IRenderableModel
{
public:
    virtual ~IRenderableModel() {}
    virtual void draw()       =0;
    virtual void drawBoundingBox() =0;
    virtual void exportToObj(const std::string&) {}
    virtual void updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj) = 0;
    virtual void updateTexturesEnabled(const bool& /* enabled */) {}
    virtual bool canExport() const {return false;}
    virtual atUint32 materialSetCount() const=0;
    virtual void setCurrentMaterialSet(atUint32)=0;
    virtual atUint32 currentMaterialSetIndex() const=0;
    virtual CMaterialSet& currentMaterialSet()=0;

    void setAmbient(float r, float g, float b)
    {
        setAmbient(QColor::fromRgbF(r, g, b));
    }

    void setAmbient(const QColor& color)
    {
        m_ambient = color;
    }

    virtual SBoundingBox& boundingBox()=0;


protected:
    QColor m_ambient;
};

#endif // IRENDERABLE_HPP
