#ifndef CSCRIPTOBJECT_HPP
#define CSCRIPTOBJECT_HPP

#include "core/CProperty.hpp"
#include <vector>


struct SConnectedObject
{
    atUint32 state;
    atUint32 message;
    CUniqueID target;
};

enum EScriptVersion
{
    eSCLY_MetroidPrime1,
    eSCLY_MetroidPrime2,
    eSCLY_MetroidPrime3,
    eSCLY_DonkeyKongCountryReturns,
};


class CModelFile;
class CScriptObject
{
public:
    CScriptObject();
    CScriptObject(Athena::io::IStreamReader& input, EScriptVersion version);
    ~CScriptObject();

    bool isAreaAttributes();
    bool skyEnabled();
    std::string typeName() const;
    void draw();
private:
    void loadStruct(Athena::io::IStreamReader &in, CStructProperty* parent, CStructPropertyTemplate* parentTemplate);
    CStructProperty* m_rootProperty;
    EScriptVersion   m_version;
    CUniqueID        m_id;
    bool             m_objectInitialized;
    CModelFile*      m_model;
    CVector3Property* m_posProperty;
    CVector3Property* m_rotProperty;
    CVector3Property* m_scaleProperty;
    std::vector<SConnectedObject> m_connectedObjects;
};

#endif // CSCRIPTOBJECT_HPP
