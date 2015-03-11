#ifndef IRENDERABLE_HPP
#define IRENDERABLE_HPP
#include "core/SBoundingBox.hpp"

#include <string>
#include <Athena/Types.hpp>
#include <QColor>

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
