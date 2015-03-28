#ifndef CPROPERTY_HPP
#define CPROPERTY_HPP

#include "core/EPropertyType.hpp"
#include "core/CPropertyTemplate.hpp"
#include "core/IResource.hpp"

#include <algorithm>
#include <memory>
#include <Athena/Global.hpp>
#include <glm/vec3.hpp>
#include <QColor>
#include <string>

class IPropertyBase
{
public:
    virtual ~IPropertyBase() {}
    virtual EPropertyType type() const =0;
    CPropertyTemplate* propertyTemplate() const;
    std::string name() const;

protected:
    friend class CScriptLoader;
    CPropertyTemplate* m_propertyTemplate;
};

template <typename T, EPropertyType _type>
class IProperty : public IPropertyBase
{
public:
    IProperty() {}
    IProperty(T v)
        : m_value(v)
    {}
    virtual ~IProperty()
    {}

    T value() const { return m_value; }
    EPropertyType type() const { return _type; }

protected:
    friend class CScriptLoader;
    T m_value;
};

typedef IProperty<std::string, String>  CStringProperty;
typedef IProperty<atUint8,     Byte  >  CByteProperty;
typedef IProperty<bool,        Bool>    CBoolProperty;
typedef IProperty<atUint32,    Long>    CLongProperty;
typedef IProperty<float,       Float>   CFloatProperty;
typedef IProperty<atUint16,    Short>   CShortProperty;
typedef IProperty<glm::vec3,   Vector3> CVector3Property;
typedef IProperty<QColor,      Color>   CColorProperty;
typedef IProperty<IResource*,  Asset>   CAssetProperty;

class CStructProperty : IPropertyBase
{
public:
    virtual ~CStructProperty();
    EPropertyType type() const { return Struct; }

    atUint32 count() const { return m_properties.size(); }
    void reserve(atUint32 count) { m_properties.reserve(count); }

    IPropertyBase* propertyByIndex(atUint32 idx);
    IPropertyBase* propertyByName(const std::string& name);

    CStructProperty* structByIndex(atUint32 idx);
    CStructProperty* structByName(const std::string& name);

    inline IPropertyBase* operator[](atUint32 idx) { return propertyByIndex(idx); }
private:
    std::vector<IPropertyBase*> m_properties;

};

#endif // CPROPERTY_HPP
