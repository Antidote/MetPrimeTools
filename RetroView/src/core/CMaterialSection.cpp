#include "core/CMaterialSection.hpp"
#include "core/CMaterialSet.hpp"
#include "core/CMaterial.hpp"

CMaterialSection::CMaterialSection()
    : m_commandImpl(nullptr)
{
}

CMaterialSection::~CMaterialSection()
{
    delete m_commandImpl;
}

SPASSCommand::SPASSCommand(Athena::io::IStreamReader& reader, CMaterialSet& set)
{
    atUint32 sectionSize = reader.readUint32();
    atUint32 sectionStart = reader.position();
    subCommand = (EMaterialCommand)reader.readUint32();
    unknown1 = reader.readUint32();
    textureId = CAssetID(reader, CAssetID::E_64Bits);
    uvSource = reader.readUint32();
    atUint32 animationSize = reader.readUint32();
    if (subCommand == EMaterialCommand::RFLD)
        std::cout << "Animation Size: " << (sectionSize - 0x18) << std::endl;
    hasAnim = (sectionSize > 0x18);
    if (hasAnim)
    {  
        animUvSource = reader.readUint16();
        postMtx = reader.readUint16();
        anim.mode = reader.readUint32();
        atUint32 parmCount = 0;
        switch(anim.mode)
        {
            case 5:
            case 4:
            case 2:
                parmCount = 4;
                break;
            case 7:
            case 3:
                parmCount = 2;
                break;
        }

        std::cout << "Parameter count for mode " << anim.mode << ": " << ((animationSize - 8) / 4) << std::endl;
        std::cout << "UVSource " << uvSource << std::endl;
        std::cout << "unk1 " << animUvSource << std::endl;
        std::cout << "unk2 " << postMtx << std::endl;
        std::cout << "values: " << std::endl;

        for (atUint32 p = 0; p < parmCount; p++)
        {
            if (anim.mode)
            anim.parms[p] = reader.readFloat();
            std::cout << std::dec << "parm " << p << "= " << anim.parms[p] << std::endl;
        }
    }

    reader.seek(sectionStart + sectionSize, Athena::SeekOrigin::Begin);
}

QStringList SPASSCommand::fragmentSource(atUint32 idx)
{
    QStringList output;

    if (textureId == CAssetID::InvalidAsset)
        return output;

    if (subCommand == EMaterialCommand::BLOL || subCommand == EMaterialCommand::RIML)
        return output;

    if (subCommand == EMaterialCommand::TRAN)
    {
        output << QString("prev = vec4(prev.rgb, 1.0 - texture(tex%1, texCoord%1.st).r);").arg(idx);
    }
    else if (subCommand == EMaterialCommand::INCA)
    {
        if (idx == 0)
            output << QString("prev = vec4(0.0, 0.0, 0.0, 1.0);");
        output << QString("prev = clamp(prev * vec4(0.5, 0.5, 0.5, 1.0) +"
                          " texture(tex%1, texCoord%1.st) + vec4(.0, .0, .0, 1), vec4(0), vec4(1));").arg(idx);
    }
    else if (subCommand == EMaterialCommand::RFLV || subCommand == EMaterialCommand::LURD)
    {
        output << QString("c0 = texture(tex%1, texCoord%1.st);").arg(idx);
    }
    else if (subCommand == EMaterialCommand::RFLD || subCommand == EMaterialCommand::LRLD)
    {
        output << QString("prev = clamp(prev * vec4(0.5, 0.5, 0.5, 1.0) + "
                          "(c0 * texture(tex%1, texCoord%1.st)) + vec4(.0, .0, .0, 1), vec4(0), vec4(1));").arg(idx);
    }
    else
    {
        output << QString("prev = prev * texture(tex%1, texCoord%1.st);").arg(idx);
    }

    return output;
}

SCLRCommand::SCLRCommand(Athena::io::IStreamReader& reader)
{
    subCommand = (EMaterialCommand)reader.readUint32();
    atUint32 tmpClr = reader.readUint32();
    Athena::utility::BigUint32(tmpClr);
    color = QColor::fromRgba(tmpClr);
    atUint8 b = color.red();
    color.setRed(color.blue());
    color.setBlue(b);
}

SINTCommand::SINTCommand(Athena::io::IStreamReader& reader)
{
    subCommand = (EMaterialCommand)reader.readUint32();
    val        = reader.readUint32();
}


atInt32 passCommandToIndex(EMaterialCommand cmd)
{
    switch(cmd)
    {
        case EMaterialCommand::DIFF:
            return 0;
        case EMaterialCommand::BLOL:
            return 1;
        case EMaterialCommand::CLR:
            return 2;
        case EMaterialCommand::INCA:
            return 3;
        case EMaterialCommand::RFLV:
            return 4;
        case EMaterialCommand::RFLD:
            return 5;
        case EMaterialCommand::LRLD:
            return 6;
        case EMaterialCommand::LURD:
            return 7;
        case EMaterialCommand::BLOD:
            return 9;
        case EMaterialCommand::BLOI:
            return 9;
        case EMaterialCommand::XRAY:
            return 10;
        case EMaterialCommand::TRAN:
            return 11;
        default:
            return -1;
    }
}
