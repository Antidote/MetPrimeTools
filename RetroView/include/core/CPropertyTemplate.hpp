#ifndef CPROPERTYTEMPLATE_HPP
#define CPROPERTYTEMPLATE_HPP

#include <string>
#include "core/EPropertyType.hpp"

class CPropertyTemplate
{
public:
    CPropertyTemplate();
    ~CPropertyTemplate();

    std::string name() const { return std::string(); }
private:
    //std::vector<CPropertyTemplate*> m_propertyTemplates;
    std::string                     m_name;
    EPropertyType                   m_type;
};

#endif // CPROPERTYTEMPLATE_HPP
