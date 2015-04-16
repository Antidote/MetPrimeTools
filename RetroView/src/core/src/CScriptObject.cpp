#include "core/CScriptObject.hpp"
#include "core/CTemplateManager.hpp"
#include "core/CResourceManager.hpp"
#include "models/CModelFile.hpp"
#include "generic/CAnimCharacterSet.hpp"
#include "ui/CGLViewer.hpp"

CScriptObject::CScriptObject()
    : m_objectInitialized(false),
      m_model(nullptr),
      m_posProperty(nullptr),
      m_rotProperty(nullptr),
      m_scaleProperty(nullptr)
{
}

CScriptObject::CScriptObject(Athena::io::IStreamReader &in, EScriptVersion version)
    : m_version(version),
      m_rootProperty(nullptr),
      m_objectInitialized(false),
      m_model(nullptr),
      m_posProperty(nullptr),
      m_rotProperty(nullptr),
      m_scaleProperty(nullptr)
{
    in.setEndian(Athena::Endian::BigEndian);
    atUint32 objType;
    if (version == eSCLY_MetroidPrime1)
    {
        objType = in.readUByte();
        atUint32 length = in.readUint32();
        atUint64 position = in.position();
        m_id = CUniqueID(in, CUniqueID::E_32Bits);

        atUint32 connectedObjectCount = in.readUint32();
        m_connectedObjects.reserve(connectedObjectCount);

        while((connectedObjectCount--) > 0)
        {
            SConnectedObject obj;
            obj.state = in.readUint32();
            obj.message = in.readUint32();
            obj.target = CUniqueID(in, CUniqueID::E_32Bits);
            m_connectedObjects.push_back(obj);
        }

        std::string typeStr = Athena::utility::sprintf("0x%X", objType);
        CStructPropertyTemplate* rootTemplate = CTemplateManager::instance()->rootTemplateByType(typeStr);
        m_rootProperty = new CStructProperty;
        if (rootTemplate)
        {
            m_rootProperty->m_propertyTemplate = rootTemplate;
            loadStruct(in, m_rootProperty, rootTemplate);
        }
        in.seek(position + length, Athena::SeekOrigin::Begin);
    }
    else
    {
        std::cout << "Only Metroid prime 1 objects at this time" << std::endl;
    }
}

void CScriptObject::loadStruct(Athena::io::IStreamReader &in, CStructProperty* parent, CStructPropertyTemplate* parentTemplate)
{
    if (parentTemplate->hasCount())
    {
        atUint32 propertyCount = in.readUint32();
        //        if (propertyCount != parentTemplate->propertyTemplates().size())
        //        {
        //            std::cout << std::dec << "Stored property count does not match expected count: " << propertyCount << " != " << parentTemplate->propertyTemplates().size() << std::endl;
        //            return;
        //        }
    }

    for (CPropertyTemplate* propertyTemplate : parentTemplate->propertyTemplates())
    {
        switch(propertyTemplate->propertyType())
        {
        case EPropertyType::Struct:
        {
            CStructProperty* prop = new CStructProperty;
            prop->m_propertyTemplate = propertyTemplate;
            loadStruct(in, prop, dynamic_cast<CStructPropertyTemplate*>(propertyTemplate));
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::Vector3:
        {
            CVector3Property* prop = new CVector3Property;
            prop->m_propertyTemplate = propertyTemplate;
            prop->m_value.x = in.readFloat();
            prop->m_value.y = in.readFloat();
            prop->m_value.z = in.readFloat();
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::Bool:
        {
            CBoolProperty* prop = new CBoolProperty;
            prop->m_propertyTemplate = propertyTemplate;
            prop->m_value = in.readBool();
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::Long:
        {
            CLongProperty* prop = new CLongProperty;
            prop->m_propertyTemplate = propertyTemplate;
            prop->m_value = in.readUint32();
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::Byte:
        {
            CByteProperty* prop = new CByteProperty;
            prop->m_propertyTemplate = propertyTemplate;
            prop->m_value = in.readByte();
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::Float:
        {
            CFloatProperty* prop = new CFloatProperty;
            prop->m_propertyTemplate = propertyTemplate;
            prop->m_value = in.readFloat();
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::String:
        {
            CStringProperty* prop = new CStringProperty;
            prop->m_propertyTemplate = propertyTemplate;
            prop->m_value = in.readString();
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::Asset:
        {
            CAssetProperty* prop = new CAssetProperty;
            prop->m_propertyTemplate = propertyTemplate;
            CUniqueID assetID;
            if (m_version == eSCLY_MetroidPrime1 || m_version == eSCLY_MetroidPrime2)
                assetID = CUniqueID(in, CUniqueID::E_32Bits);
            else
                assetID = CUniqueID(in, CUniqueID::E_64Bits);

            prop->m_value = assetID;
            parent->m_properties.push_back(prop);
        }
            break;
        case EPropertyType::Color:
        {
            CColorProperty* prop = new CColorProperty;
            prop->m_propertyTemplate = propertyTemplate;
            prop->m_value.setRedF(in.readFloat());
            prop->m_value.setGreenF(in.readFloat());
            prop->m_value.setBlueF(in.readFloat());
            prop->m_value.setAlphaF(in.readFloat());
            parent->m_properties.push_back(prop);
        }
            break;
        default:
            std::cout << "todo" << std::endl;
        }
    }
}

CScriptObject::~CScriptObject()
{
}

bool CScriptObject::isAreaAttributes()
{
    return !m_rootProperty->name().compare("AreaAttributes");
}

bool CScriptObject::skyEnabled()
{
    CBoolProperty* useSkyboxProp = dynamic_cast<CBoolProperty*>(m_rootProperty->propertyByName("Skybox toggle"));
    if (useSkyboxProp)
        return useSkyboxProp->value();

    return false;
}

std::string CScriptObject::typeName() const
{
    return m_rootProperty->name();
}

void CScriptObject::draw()
{
    if (!m_rootProperty)
        return;

    if (m_model == nullptr && !m_objectInitialized)
    {
        CAssetProperty* characterSetProp   = dynamic_cast<CAssetProperty*>(m_rootProperty->propertyByName("AnimationParameters::AnimSet"));
        if (!characterSetProp)
            characterSetProp   = dynamic_cast<CAssetProperty*>(m_rootProperty->propertyByName("PatternedInfo::AnimationParameters::AnimSet"));

        if (characterSetProp)
        {
            CAnimCharacterSet* charSet = dynamic_cast<CAnimCharacterSet*>(characterSetProp->load());

            if (charSet)
            {
                atUint32 nodeId = 0;
                if (m_rootProperty->name() != "PlayerActor")
                {
                    CLongProperty* nodeProp = dynamic_cast<CLongProperty*>(m_rootProperty->propertyByName("PatternedInfo::AnimationParameters::Node"));

                    if (!nodeProp)
                        nodeProp = dynamic_cast<CLongProperty*>(m_rootProperty->propertyByName("AnimationParameters::Node"));
                    if (nodeProp)
                        nodeId = nodeProp->value();
                }

                CAnimCharacterNode* node = charSet->nodeByIndex(nodeId);
                if (node)
                    m_model = dynamic_cast<CModelFile*>(CResourceManager::instance()->loadResource(node->modelID(), "CMDL"));
            }
        }

        if (!m_model)
        {
            CAssetProperty* assetProp = dynamic_cast<CAssetProperty*>(m_rootProperty->propertyByName("Model"));
            if (assetProp)
                m_model = dynamic_cast<CModelFile*>(assetProp->load());
        }

        m_posProperty    = dynamic_cast<CVector3Property*>(m_rootProperty->propertyByName("Position"));
        m_rotProperty    = dynamic_cast<CVector3Property*>(m_rootProperty->propertyByName("Rotation"));
        m_scaleProperty  = dynamic_cast<CVector3Property*>(m_rootProperty->propertyByName("Scale"));
        if (!m_scaleProperty)
            m_scaleProperty = dynamic_cast<CVector3Property*>(m_rootProperty->propertyByName("Volume"));
        m_objectInitialized = true;
    }

    if (m_model)
    {
        if (m_posProperty)
            m_model->setPosition(m_posProperty->value());
        if (m_rotProperty)
            m_model->setRotation(m_rotProperty->value());
        if (m_scaleProperty)
            m_model->setScale(m_scaleProperty->value());
        m_model->updateViewProjectionUniforms(CGLViewer::instance()->view(), CGLViewer::instance()->projection());
        m_model->draw();
        m_model->restoreDefaults();
    }
}

CVector3f CScriptObject::position()
{
    if (!m_posProperty)
        m_posProperty = dynamic_cast<CVector3Property*>(m_rootProperty->propertyByName("Position"));

    return m_posProperty->value();
}

CVector3f CScriptObject::rotation()
{
    if (!m_rotProperty)
        m_rotProperty = dynamic_cast<CVector3Property*>(m_rootProperty->propertyByName("Rotation"));
    return m_rotProperty->value();
}

