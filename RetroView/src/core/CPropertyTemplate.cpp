#include "core/CPropertyTemplate.hpp"
#include <algorithm>
#include <memory>

CPropertyTemplate::CPropertyTemplate()
{
}

CPropertyTemplate::~CPropertyTemplate()
{
}


CAssetPropertyTemplate::CAssetPropertyTemplate()
{
    m_propertyType = EPropertyType::Asset;
}

CAssetPropertyTemplate::~CAssetPropertyTemplate()
{
}

CFourCC CAssetPropertyTemplate::assetType() const
{
    return m_assetType;
}


CStructPropertyTemplate::CStructPropertyTemplate()
{
    m_propertyType = EPropertyType::Struct;
}

CStructPropertyTemplate::~CStructPropertyTemplate()
{
    std::for_each(m_propertyTemplates.begin(), m_propertyTemplates.end(), std::default_delete<CPropertyTemplate>());
}

std::vector<CPropertyTemplate*> CStructPropertyTemplate::propertyTemplates() const
{
    return m_propertyTemplates;
}

