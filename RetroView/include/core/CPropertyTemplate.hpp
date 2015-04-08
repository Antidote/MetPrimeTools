#ifndef CPROPERTYTEMPLATE_HPP
#define CPROPERTYTEMPLATE_HPP

#include <string>
#include <vector>

#include <CFourCC.hpp>
#include "core/EPropertyType.hpp"

class CPropertyTemplate
{
public:
    CPropertyTemplate();
    virtual ~CPropertyTemplate();

    std::string name()           const { return m_name; }
    EPropertyType propertyType() const { return m_propertyType; }
protected:
    friend class CTemplateManager;
    std::string                     m_name;
    EPropertyType                   m_propertyType;
};

class CAssetPropertyTemplate : public CPropertyTemplate
{
public:
    CAssetPropertyTemplate();
    virtual ~CAssetPropertyTemplate();

    CFourCC assetType() const;
private:
    friend class CTemplateManager;
    CFourCC m_assetType;
};

class CStructPropertyTemplate : public CPropertyTemplate
{
public:
    CStructPropertyTemplate();
    virtual ~CStructPropertyTemplate();

    bool hasCount() const { return m_hasCount; }
    std::vector<CPropertyTemplate*> propertyTemplates() const;
private:
    friend class CTemplateManager;
    bool m_hasCount;
    std::vector<CPropertyTemplate*> m_propertyTemplates; // child properties
};

#endif // CPROPERTYTEMPLATE_HPP
