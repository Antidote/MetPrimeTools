#include "core/CTemplateManager.hpp"
#include "core/CPropertyTemplate.hpp"
#include <sstream>
#include <tinyxml.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

struct concrete_CTemplateManager : public CTemplateManager
{
    concrete_CTemplateManager()
    {}
};

void CTemplateManager::initialize(const std::string& templatePath)
{
    std::cout << "Initializing template manager @ " << templatePath << std::endl;
    m_templatePath = templatePath;
    loadTemplates();
}

std::shared_ptr<CTemplateManager> CTemplateManager::instance()
{
    static std::shared_ptr<CTemplateManager> _instance = std::make_shared<concrete_CTemplateManager>();

    return _instance;
}

CPropertyTemplate* CTemplateManager::propertyTemplate(CTemplateManager::EPropertyVersion version, const std::string& type)
{
}

CTemplateManager::CTemplateManager()
{
    std::cout << "Template Manager created" << std::endl;
}

CTemplateManager::~CTemplateManager()
{
}

void CTemplateManager::loadTemplates()
{
    loadMasterTemplate(MP1_TEMPLATE_DIR);
    //loadMasterTemplate(MP2_TEMPLATE_DIR);
}

void CTemplateManager::loadTemplate(const std::string& templatePath)
{
}


void CTemplateManager::loadMasterTemplate(std::string gamePath)
{
    TiXmlDocument masterTemplate(m_templatePath + gamePath + MASTER_TEMPLATE_FILENAME);
    if (masterTemplate.LoadFile())
    {
        std::cout << "Loaded Master Template" << std::endl;

        TiXmlElement* root = masterTemplate.RootElement();
        if (root->ValueStr() != "MasterTemplate")
            std::cout << "Master template invalid" << std::endl;
        else
        {
            TiXmlElement* element = root->FirstChildElement("object");
            while (element != nullptr)
            {
                if (element->Attribute("ID") == NULL || element->Attribute("name") == NULL)
                    continue;

                std::string id = std::string(element->Attribute("ID"));
                std::string name = std::string(element->Attribute("name"));
                std::cout << id << " " << name << std::endl;

                element = element->NextSiblingElement("object");
            }
        }
    }
}
