#ifndef CSCRIPTOBJECT_HPP
#define CSCRIPTOBJECT_HPP

#include "core/CProperty.hpp"
#include <vector>

struct SConnectedObject
{
    atUint32 state;
    atUint32 message;
    CAssetID target;
};

enum EScriptVersion
{
    eSCLY_MetroidPrime1,
    eSCLY_MetroidPrime2,
    eSCLY_MetroidPrime3,
    eSCLY_DonkeyKongCountryReturns,
};


class CScriptObject
{
public:
    CScriptObject();
    CScriptObject(Athena::io::IStreamReader& input, EScriptVersion version);
    ~CScriptObject();

    void draw();
private:
    void loadStruct(Athena::io::IStreamReader &in, CStructProperty* parent, CStructPropertyTemplate* parentTemplate);
    CStructProperty m_rootProperty;
    EScriptVersion  m_version;
    CAssetID        m_id;
    std::vector<SConnectedObject> m_connectedObjects;
};

#endif // CSCRIPTOBJECT_HPP
