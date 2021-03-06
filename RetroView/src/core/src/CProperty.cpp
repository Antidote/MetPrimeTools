#include "core/CProperty.hpp"
#include "core/CResourceManager.hpp"
#include <algorithm>
#include <memory>

CPropertyTemplate* IPropertyBase::propertyTemplate() const
{
    return m_propertyTemplate;
}

std::string IPropertyBase::name() const
{
    return m_propertyTemplate->name();
}

CStructProperty::~CStructProperty()
{
    std::for_each(m_properties.begin(), m_properties.end(), std::default_delete<IPropertyBase>());
}

IPropertyBase* CStructProperty::propertyByIndex(atUint32 idx)
{
    if (idx >= m_properties.size())
        return nullptr;

    return m_properties[idx];
}

IPropertyBase* CStructProperty::propertyByName(const std::string& name)
{
    size_t nsStart = name.find_first_of("::");
    size_t propStart = nsStart + 2;

    if (nsStart != std::string::npos)
    {
        std::string structName = name.substr(0, nsStart);
        std::string propName = name.substr(propStart, name.length() - propStart);

        CStructProperty *st = structByName(structName);
        if (!st)
            return nullptr;
        else
            return st->propertyByName(propName);
    }
    else
    {
        for (IPropertyBase* prop : m_properties)
        {
            if (prop && prop->name() == name)
                return prop;
        }
        return nullptr;
    }
}

CStructProperty* CStructProperty::structByIndex(atUint32 idx)
{
    return dynamic_cast<CStructProperty*>(propertyByIndex(idx));
}

CStructProperty* CStructProperty::structByName(const std::string& name)
{
    return dynamic_cast<CStructProperty*>(propertyByName(name));
}


IResource* CAssetProperty::load()
{
    CAssetPropertyTemplate* assetTemplate = dynamic_cast<CAssetPropertyTemplate*>(m_propertyTemplate);
    return CResourceManager::instance()->loadResource(m_value, assetTemplate->assetType().toString());
}
