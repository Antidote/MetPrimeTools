#ifndef CTEMPLATEMANAGER_HPP
#define CTEMPLATEMANAGER_HPP

#include <Athena/Global.hpp>
#include <unordered_map>
#include <string>
#include <memory>

#define MP1_TEMPLATE_DIR         "/mp1/"
#define MP2_TEMPLATE_DIR         "/mp2/"
#define MASTER_TEMPLATE_FILENAME "MasterTemplate.xml"

class TiXmlDocument;
class TiXmlElement;
class CPropertyTemplate;
class CStructPropertyTemplate;
class CTemplateManager
{
public:
    void initialize(const std::string& templatePath);
    static std::shared_ptr<CTemplateManager> instance();

    CStructPropertyTemplate* rootTemplateByType(const std::string& type);

protected:
    CTemplateManager();
    ~CTemplateManager();
private:
    void loadTemplates();
    CPropertyTemplate* loadProperty(TiXmlElement* child);
    CPropertyTemplate* loadStruct(TiXmlElement* child);
    void loadMasterTemplate(const std::string& tmpPath);
    CStructPropertyTemplate* loadTemplate(const std::string& templatePath, const std::string& name);
    std::unordered_map<std::string, CStructPropertyTemplate*> m_templateRoots;
    std::string m_templatePath;
};

#endif // CTEMPLATEMANAGER_HPP
