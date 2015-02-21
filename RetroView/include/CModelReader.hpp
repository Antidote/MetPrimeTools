#ifndef CMDLREADER_HPP
#define CMDLREADER_HPP


#include <Athena/BinaryReader.hpp>
#include "CResourceManager.hpp"

class CModelFile;
class CModelReader final : protected Athena::io::BinaryReader
{
    BINARYREADER_BASE();
    DEFINE_RESOURCE_LOADER();
public:
    CModelReader(const atUint8* data, atUint64 length);
    CModelReader(const std::string& filepath);
    virtual ~CModelReader();

    CModelFile* read();
    static IResource* loadByData(const atUint8* data, atUint64 length);

private:
    void readVertices   (Athena::io::BinaryReader& in);
    void readNormals    (Athena::io::BinaryReader& in);
    void readColors     (Athena::io::BinaryReader& in);
    void readTexCoords  (atUint32 slot, Athena::io::BinaryReader& in);
    void readMeshOffsets(Athena::io::BinaryReader& in);
    void readMesh       (Athena::io::BinaryReader& in);

    CModelFile*             m_result;
    std::vector<atUint32> m_sectionSizes;
    std::vector<atUint32> m_meshOffsets;
};

#endif // CMDLREADER_HPP
