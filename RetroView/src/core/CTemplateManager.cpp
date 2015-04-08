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
    Athena::utility::toupper(tmp);
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

CPropertyTemplate* CTemplateManager::loadProperty(TiXmlElement* child)
{
    CPropertyTemplate* childProp = nullptr;
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
        return childProp;
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
    else if (!type.compare("byte"))
        childProp->m_propertyType = EPropertyType::Byte;
    else if (!type.compare("enum"))
        childProp->m_propertyType = EPropertyType::Enum;
    else if (!type.compare("array"))
        childProp->m_propertyType = EPropertyType::Array;

    return childProp;
}

CPropertyTemplate* CTemplateManager::loadStruct(TiXmlElement* child)
{
    CPropertyTemplate* childProp = nullptr;
    std::string childName = child->Attribute("name");
    const char* tmp = child->Attribute("template");
    if (tmp == nullptr)
    {
        childProp = new CStructPropertyTemplate();
        CStructPropertyTemplate* strTemp = dynamic_cast<CStructPropertyTemplate*>(childProp);
        strTemp->m_name = childName;
        strTemp->m_hasCount = (child->FirstChild("count") != nullptr);

        TiXmlElement* subChild = child->FirstChildElement();;
        while (subChild != nullptr)
        {
            CPropertyTemplate* subChildProp = nullptr;
            if (!subChild->ValueStr().compare("property"))
            {
                subChildProp = loadProperty(subChild);
            }
            else if (!subChild->ValueStr().compare("struct"))
            {
                subChildProp = loadStruct(subChild);
            }

            if (subChildProp)
                strTemp->m_propertyTemplates.push_back(subChildProp);

            subChild = subChild->NextSiblingElement();
        }
    }
    else
    {
        childProp = loadTemplate("/" + std::string(tmp), childName);
    }

    return childProp;
}

CStructPropertyTemplate* CTemplateManager::loadTemplate(const std::string& templatePath, const std::string& name)
{
    CStructPropertyTemplate* ret = nullptr;
    TiXmlDocument objTemplate(m_templatePath + templatePath);
    if (objTemplate.LoadFile())
    {
        TiXmlElement* root = objTemplate.RootElement();
        ret = new CStructPropertyTemplate;
        ret->m_name = name;
        ret->m_hasCount = (root->FirstChild("count") != nullptr);

        TiXmlElement* child = root->FirstChildElement();

        while (child != nullptr)
        {
            CPropertyTemplate* childProp = nullptr;
            if (!child->ValueStr().compare("property"))
            {
                childProp = loadProperty(child);
            }
            else if (!child->ValueStr().compare("struct"))
            {
                childProp = loadStruct(child);
            }

            if (childProp)
                ret->m_propertyTemplates.push_back(childProp);
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
                Athena::utility::toupper(id);
                if (objTemplate != nullptr)
                    m_templateRoots[id] = objTemplate;
                else
                    std::cout << "Failed to load template for " << name << std::endl;
                element = element->NextSiblingElement("object");
            }
        }
    }
}
