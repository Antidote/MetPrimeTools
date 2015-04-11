#ifndef CPROPERTY_HPP
#define CPROPERTY_HPP

#include "core/EPropertyType.hpp"
#include "core/CPropertyTemplate.hpp"
#include "core/IResource.hpp"

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
    friend class CScriptObject;
    T m_value;
};

class CStringProperty : public IProperty<std::string, String>
{
public:
    virtual ~CStringProperty() {}
};

class CByteProperty : public IProperty<atUint8, Byte>
{
public:
    virtual ~CByteProperty() {}
};

class CBoolProperty : public IProperty<bool, Bool>
{
public:
    virtual ~CBoolProperty() {}
};

class CLongProperty : public IProperty<atUint32, Long>
{
public:
    virtual ~CLongProperty() {}
};

class CFloatProperty : public IProperty<float,  Float>
{
public:
    virtual ~CFloatProperty() {}
};

class CShortProperty : public IProperty<atUint16, Short>
{
public:
    virtual ~CShortProperty() {}
};

class CVector3Property : public IProperty<glm::vec3, Vector3>
{
public:
    virtual ~CVector3Property() {}
};

class CColorProperty : public IProperty<QColor, Color>
{
public:
    virtual ~CColorProperty() {}
};

class CAssetProperty : public IProperty<CUniqueID,  Asset>
{
public:
    virtual ~CAssetProperty() {}

    IResource* load();
private:
};

class CStructProperty : public IPropertyBase
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
    friend class CScriptObject;
    std::vector<IPropertyBase*> m_properties;

};

#endif // CPROPERTY_HPP
