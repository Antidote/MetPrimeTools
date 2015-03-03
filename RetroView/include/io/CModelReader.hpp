#ifndef CMDLREADER_HPP
#define CMDLREADER_HPP

#include "core/CResourceManager.hpp"

#include <Athena/MemoryReader.hpp>

class CModelFile;
class CModelReader final : protected Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
    DEFINE_RESOURCE_LOADER();
public:
    CModelReader(const atUint8* data, atUint64 length);
    CModelReader(const std::string& filepath);
    virtual ~CModelReader();

    CModelFile* read();
    static IResource* loadByData(const atUint8* data, atUint64 length);

private:
    void readVertices   (Athena::io::MemoryReader& in);
    void readNormals    (Athena::io::MemoryReader& in);
    void readColors     (Athena::io::MemoryReader& in);
    void readTexCoords  (atUint32 slot, Athena::io::MemoryReader& in);
    void readMeshOffsets(Athena::io::MemoryReader& in);
    void readMesh       (Athena::io::MemoryReader& in);

    CModelFile*             m_result;
    std::vector<atUint32> m_sectionSizes;
    std::vector<atUint32> m_meshOffsets;
};

#endif // CMDLREADER_HPP
