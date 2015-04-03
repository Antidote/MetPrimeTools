#include "core/CTemplate.hpp"
#include "core/CPropertyTemplate.hpp"
#include <algorithm>

CTemplate::CTemplate()
{

}

CTemplate::~CTemplate()
{

}

CPropertyTemplate*CTemplate::propertyTemplateByName(const std::string& name)
{
    auto iter = std::find_if(m_properties.begin(), m_properties.end(),
                             [&name](CPropertyTemplate* tmp)->bool
    {
            return !tmp->name().compare(name);
    });

    if (iter != m_properties.end())
        return *iter;

    return nullptr;
}

