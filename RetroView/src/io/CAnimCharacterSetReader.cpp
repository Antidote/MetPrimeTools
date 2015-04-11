#include "io/CAnimCharacterSetReader.hpp"
#include "generic/CAnimCharacterSet.hpp"
#include <RetroCommon.hpp>
#include <Athena/MemoryWriter.hpp>
#include <Athena/InvalidDataException.hpp>

CAnimCharacterSetReader::CAnimCharacterSetReader(const atUint8* data, atUint64 length)
    : base(data, length)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CAnimCharacterSetReader::CAnimCharacterSetReader(const std::string& filepath)
    : base(filepath)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CAnimCharacterSetReader::~CAnimCharacterSetReader()
{
}


CAnimCharacterSet* CAnimCharacterSetReader::read()
{
    CAnimCharacterSet* ret = nullptr;
    atUint16 unk1 = base::readUint16();
    atUint16 unk2 = base::readUint16();

    if (unk1 != 1 || unk2 != 1)
    {
        Athena::io::MemoryWriter tmp;
        decompressFile(tmp, base::data(), base::length());

        if (tmp.length() > 0x10)
            base::setData(tmp.data(), tmp.length());

        unk1 = base::readUint16();
        unk2 = base::readUint16();
    }

    if (unk1 != 1 || unk2 != 1)
        THROW_INVALID_DATA_EXCEPTION("Expected unknowns 1 and 2 to be 1, got %i and %i instead", unk1, unk2);

    try
    {
        ret = new CAnimCharacterSet;
        atUint32 nodeCount = base::readUint32();

        while ((nodeCount--) > 0)
        {
            readCharacterNode(ret);
        }
    }
    catch(...)
    {
        delete ret;
        throw;
    }

    return ret;
}



void CAnimCharacterSetReader::readCharacterNode(CAnimCharacterSet* ret)
{
    CAnimCharacterNode* node = new CAnimCharacterNode;
    node->m_id = base::readUint32();
    node->m_enumVal = base::readUint16();

    node->m_name    = base::readString();
    node->m_modelId = CUniqueID(*this, CUniqueID::E_32Bits);
    node->m_skinId  = CUniqueID(*this, CUniqueID::E_32Bits);
    node->m_rigId   = CUniqueID(*this, CUniqueID::E_32Bits);

    readActions(node);

    node->m_pasDatabase = new CPASDatabase(*this);

    atUint32 particleCount = base::readUint32();

    while((particleCount--) > 0)
        node->m_particleIds.emplace_back(CUniqueID(*this, CUniqueID::E_32Bits));

    atUint32 swooshCount = base::readUint32();
    while((swooshCount--) > 0)
        node->m_swooshIds.emplace_back(CUniqueID(*this, CUniqueID::E_32Bits));

    if (node->m_enumVal > 4)
    {
        atUint32 unkCount = base::readUint32();
        while((unkCount--) > 0)
            node->m_unkIds.emplace_back(CUniqueID(*this, CUniqueID::E_32Bits));
    }

    if (node->m_enumVal > 5)
    {
        atUint32 electricCount = base::readUint32();
        while((electricCount--) > 0)
            node->m_electricIds.emplace_back(CUniqueID(*this, CUniqueID::E_32Bits));
    }

    node->m_attachementMeta = base::readUint32();

    atUint32 actionAABBCount = base::readUint32();
    while((actionAABBCount--) > 0)
        readActionBoundingBox(node);

    atUint32 attachmentCount = base::readUint32();
    while ((attachmentCount--) > 0)
        readAttachement(node);

    node->m_modelOverride = CUniqueID(*this, CUniqueID::E_32Bits);
    node->m_skinOverride  = CUniqueID(*this, CUniqueID::E_32Bits);

    atUint32 actionIDCount = base::readUint32();
    while ((actionIDCount--) > 0)
        node->m_actionIds.push_back(base::readUint32());
    ret->m_characterNodes.push_back(node);
}

void CAnimCharacterSetReader::readActions(CAnimCharacterNode* node)
{
    atUint32 actionCount = base::readUint32();
    while ((actionCount--) > 0)
    {
        SAction action;
        action.id = base::readUint32();
        action.unk = base::readUByte();
        action.name = base::readString();
        node->m_actions.push_back(action);
    }
}

void CAnimCharacterSetReader::readAttachement(CAnimCharacterNode* node)
{
    SEffectAttachement attachment;
    attachment.name = base::readString();
    atUint32 subAttachmentCount = base::readUint32();
    while((subAttachmentCount--) > 0)
    {
        SSubEffectAttachement subAttachment;
        subAttachment.name = base::readString();
        subAttachment.type = CFourCC(*this);
        subAttachment.id =  CUniqueID(*this, CUniqueID::E_32Bits);
        subAttachment.name2 = base::readString();
        subAttachment.unkFloat = base::readFloat();
        subAttachment.unkInt1 = base::readUint32();
        subAttachment.unkInt2 = base::readUint32();
        attachment.subAttachments.push_back(subAttachment);
    }
    node->m_effectAttachments.push_back(attachment);
}

void CAnimCharacterSetReader::readActionBoundingBox(CAnimCharacterNode* node)
{
    SActionAABB aabb;
    aabb.name = base::readString();
    aabb.aabb = SBoundingBox(*this);
    node->m_actionAABBs.push_back(aabb);
}

IResource* CAnimCharacterSetReader::loadByData(const atUint8* data, atUint64 length)
{
    return CAnimCharacterSetReader(data, length).read();
}

REGISTER_RESOURCE_LOADER(CAnimCharacterSetReader, "ancs", loadByData);

