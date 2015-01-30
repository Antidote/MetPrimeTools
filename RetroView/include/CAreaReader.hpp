#ifndef MREAREADER_HPP
#define MREAREADER_HPP

#include <Athena/BinaryReader.hpp>
#include "CResourceManager.hpp"

struct SAreaSectionIndex final
{
    std::string tag;
    atUint32    index;
};


class CMaterial;
class CModelData;
class CMesh;
class CAreaFile;
class IRenderableModel;
class CAreaReader final : protected Athena::io::BinaryReader
{
    BINARYREADER_BASE();
    DEFINE_RESOURCE_LOADER();
public:
    CAreaReader(const atUint8* data, atUint64 length);
    CAreaReader(const std::string& filepath);
    virtual ~CAreaReader();

    CAreaFile* read();

    static IResource*        loadByFile(const std::string& filepath);
    static IResource*        loadByData(const atUint8* data, atUint64 length);

private:
    void readSections(CAreaFile* ret);
    void readSectionsMP3DKCR(CAreaFile* ret);
    void readModelHeader(CAreaFile*  file, atUint64& sectionStart, atUint32& i);
    void readModelData  (CAreaFile* file,  CModelData& model, atUint64& sectionStart, atUint32& i);
    void readMeshOffsets(CModelData& model, Athena::io::BinaryReader& in);
    void readAROT       (CAreaFile*  file,  Athena::io::BinaryReader& in);
    void readAABB       (CAreaFile*  file,  Athena::io::BinaryReader& in);
    void readVertices   (CModelData& model, Athena::io::BinaryReader& in);
    void readNormals    (CModelData& model, Athena::io::BinaryReader& in);
    void readColors     (CModelData& model, Athena::io::BinaryReader& in);
    void readTexCoords  (CModelData& model, Athena::io::BinaryReader& in, bool isLightmap);
    void readMesh       (CModelData& model, CAreaFile* ret, Athena::io::BinaryReader& in);
    void readMeshes     (CAreaFile* ret, CModelData& model, atUint64& sectionStart, atUint32& i, atUint32 m);

    std::vector<std::vector<atUint32>> m_modelMeshOffsets;
    std::vector<atUint32>    m_sectionSizes;
    Athena::io::BinaryReader m_sectionReader;
    // The following is for Metroid Prime 1 and 2 only:
    atUint32                 m_materialSection;
    atUint32                 m_sclySection;
    atUint32                 m_scgnSection; // Not in metroid Prime
    atUint32                 m_collisionSection;
    atUint32                 m_unknownSection; // seriously wtf does this do?
    atUint32                 m_lightSection;
    atUint32                 m_visiSection;
    atUint32                 m_pathSection;
    atUint32                 m_arotSection;
    atUint32                 m_ptlaSection; // Not in metroid Prime
    atUint32                 m_egmcSection; // Not in metroid Prime

    // Same information, just for MP3 and DKCR
    std::vector<SAreaSectionIndex> m_sectionIndices;

};

#endif // MREAREADER_HPP
