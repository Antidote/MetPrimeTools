#ifndef CMATERIALSECTION_HPP
#define CMATERIALSECTION_HPP

#include <CAssetID.hpp>
#include <QColor>
#include "core/SAnimation.hpp"

enum class EMaterialCommand
{
    BLOD = 0x424C4F44,
    BLOI = 0x424C4F49,
    BLOL = 0x424C4F4C,
    BNIF = 0x424E4946,
    CLR  = 0x434C5220,
    DIFF = 0x44494646,
    DIFB = 0x44494642,
    END  = 0x454E4420,
    INCA = 0x494E4341,
    INT  = 0x494E5420,
    LURD = 0x4C555244,
    LRLD = 0x4C524C44,
    OPAC = 0x4F504143,
    PASS = 0x50415353,
    RFLD = 0x52464c44,
    RFLV = 0x52464c56,
    RIML = 0x52494D4C,
    TRAN = 0x5452414E,
    XRAY = 0x58524159
};

struct IMaterialCommand
{
    virtual ~IMaterialCommand() {}
    EMaterialCommand subCommand;
};

class CMaterialSet;
struct SPASSCommand final : IMaterialCommand
{
    SPASSCommand(Athena::io::IStreamReader& reader, CMaterialSet& set);

    virtual ~SPASSCommand() {}
    atUint32 unknown1;
    CAssetID textureId;
    atUint32 uvSource;
    atUint16 animUvSource;
    atUint16 postMtx;
    SAnimation anim;
    bool hasAnim;
    QStringList fragmentSource(atUint32 idx);
};

struct SCLRCommand final : IMaterialCommand
{
    SCLRCommand(Athena::io::IStreamReader& reader);
    virtual ~SCLRCommand() {}
    // TODO: Create custom Color class
    QColor color;
};

struct SINTCommand final : IMaterialCommand
{
    SINTCommand(Athena::io::IStreamReader& reader);

    virtual ~SINTCommand() {}
    atUint32 val;
};

class CMaterialSection final
{
public:
    CMaterialSection();
    ~CMaterialSection();
private:
    friend class CMaterialReader;
    friend class CMaterial;
    EMaterialCommand  m_command;
    IMaterialCommand* m_commandImpl;
};


atInt32 passCommandToIndex(EMaterialCommand cmd);

#endif // CMATERIALPASS_HPP

