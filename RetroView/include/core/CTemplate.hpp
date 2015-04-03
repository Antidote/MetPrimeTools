#ifndef CTEMPLATE_HPP
#define CTEMPLATE_HPP

#include <string>
#include <vector>

struct SAttribute
{
    std::string type;
    std::string target;
};

class CPropertyTemplate;
class CTemplate
{
public:
    CTemplate();
    ~CTemplate();

    CPropertyTemplate* propertyTemplateByName(const std::string& name);
private:
    std::vector<CPropertyTemplate*> m_properties;
};

#endif // CTEMPLATE_HPP
