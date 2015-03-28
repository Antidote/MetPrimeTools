#ifndef CTEMPLATEMANAGER_HPP
#define CTEMPLATEMANAGER_HPP

#include <Athena/Global.hpp>
#include <unordered_map>
#include <string>
#include <memory>

#define MP1_TEMPLATE_DIR        "/mp1"
#define MP2_TEMPLATE_DIR        "/mp2"
#define MASTER_TEMPLATE_FILENAME "/MasterTemplate.xml"

class TiXmlDocument;
class CPropertyTemplate;
class CTemplateManager
{
public:
    enum EPropertyVersion
    {
        MetroidPrime1,
        MetroidPrime2
    };

    void initialize(const std::string& templatePath);
    static std::shared_ptr<CTemplateManager> instance();

    CPropertyTemplate* propertyTemplate(EPropertyVersion version, const std::string& type);
protected:
    CTemplateManager();
    ~CTemplateManager();
private:
    void loadTemplates();
    void loadMasterTemplate(std::string tmpPath, EPropertyVersion version);
    std::unordered_map<std::string, std::string> m_templatePaths;
    std::string m_templatePath;
};

#endif // CTEMPLATEMANAGER_HPP
