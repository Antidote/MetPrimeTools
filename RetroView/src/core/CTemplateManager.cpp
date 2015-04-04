#include "core/CTemplateManager.hpp"
#include "core/CPropertyTemplate.hpp"
#include <memory>
#include <algorithm>
#include <tinyxml.h>
#include <iostream>

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

CStructPropertyTemplate* CTemplateManager::rootTemplateByType(const std::string& type)
{
    std::string tmp(type);
    Athena::utility::tolower(tmp);
    if (m_templateRoots.find(tmp) != m_templateRoots.end())
        return m_templateRoots[tmp];

    return nullptr;
}

CTemplateManager::CTemplateManager()
{
    std::cout << "Template Manager created" << std::endl;
}

CTemplateManager::~CTemplateManager()
{
    for (std::pair<std::string, CStructPropertyTemplate*> pt : m_templateRoots)
        delete pt.second;

    m_templateRoots.clear();
}

void CTemplateManager::loadTemplates()
{
    loadMasterTemplate(MP1_TEMPLATE_DIR);
    //loadMasterTemplate(MP2_TEMPLATE_DIR);
}

CStructPropertyTemplate* CTemplateManager::loadTemplate(const std::string& templatePath, const std::string& name)
{
    CStructPropertyTemplate* ret = nullptr;
    std::cout << templatePath << std::endl;
    TiXmlDocument objTemplate(m_templatePath + templatePath);
    if (objTemplate.LoadFile())
    {
        TiXmlElement* root = objTemplate.RootElement();
        ret = new CStructPropertyTemplate;
        ret->m_name = name;

        TiXmlElement* countElement = root->FirstChildElement("count");
        if (countElement == nullptr)
            return nullptr;


        TiXmlElement* child = root->FirstChildElement();

        while (child != nullptr)
        {
            CPropertyTemplate* childProp = nullptr;
            if (!child->ValueStr().compare("property"))
            {
                std::string childName = child->Attribute("name");
                std::string type = child->Attribute("type");
                Athena::utility::tolower(type);
                if (type == "file")
                {
                    childProp = new CAssetPropertyTemplate;
                    CAssetPropertyTemplate* assetProp = dynamic_cast<CAssetPropertyTemplate*>(childProp);
                    assetProp->m_assetType = child->Attribute("ext");
                    assetProp->m_name = childName;
                    child = child->NextSiblingElement();
                    ret->m_propertyTemplates.push_back(childProp);
                    continue;
                }
                else
                {
                    childProp = new CPropertyTemplate;
                    childProp->m_name = childName;
                }

                if (!type.compare("string"))
                    childProp->m_propertyType = EPropertyType::String;
                else if (!type.compare("vector3f"))
                    childProp->m_propertyType = EPropertyType::Vector3;
                else if (!type.compare("float"))
                    childProp->m_propertyType = EPropertyType::Float;
                else if (!type.compare("bool"))
                    childProp->m_propertyType = EPropertyType::Bool;
                else if (!type.compare("long"))
                    childProp->m_propertyType = EPropertyType::Long;
                else if (!type.compare("file"))
                    childProp->m_propertyType = EPropertyType::Asset;
                else if (!type.compare("color"))
                    childProp->m_propertyType = EPropertyType::Color;

                ret->m_propertyTemplates.push_back(childProp);
            }
            else if (!child->ValueStr().compare("struct"))
            {
                std::string childName = child->Attribute("name");
                std::string path = child->Attribute("template");
                childProp = loadTemplate("/" + path, childName);
                if (childProp)
                {
                    ret->m_propertyTemplates.push_back(childProp);
                }
            }

            child = child->NextSiblingElement();
        }
    }

    return ret;
}

void CTemplateManager::loadMasterTemplate(const std::string& gamePath)
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
                CStructPropertyTemplate* objTemplate = loadTemplate(gamePath + name + ".xml", name);
                Athena::utility::tolower(id);
                if (objTemplate != nullptr)
                    m_templateRoots[id] = objTemplate;
                std::cout << id << " " << name << std::endl;
                element = element->NextSiblingElement("object");
            }
        }
    }
}
