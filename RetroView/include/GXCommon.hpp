#ifndef GXCOMMON_HPP
#define GXCOMMON_HPP

#include <Athena/Global.hpp>
#include <glm/glm.hpp>
#include "SBoundingBox.hpp"
#include "GXTypes.hpp"

#define KCOLORS "konst"

void drawWireSphere(float radius, atUint32 longs, atUint32 lats);
void drawSolidSphere(double radius, atUint32 slices, atUint32 stacks);
void drawOutlinedCube(const glm::vec3& position, const glm::vec4& mainColor, const glm::vec4& outlineColor, const glm::vec3& scale = glm::vec3(1));
void drawTexturedCube(atUint32 texture, const glm::vec3& position, float alpha, const glm::vec3& scale = glm::vec3(1));
void drawBoundingBox(SBoundingBox bbox);
void GXSetBlendMode(EBlendMode src, EBlendMode dst);
void drawCone(float width, float height, float offset);
void drawAxis(glm::vec3 translation, glm::vec3 orientation, float scale, bool disableDepth = true);
atUint64 assetIdFromPath(const std::string& filepath);

class CMesh;
class CMaterial;

namespace Athena
{
namespace io
{
class BinaryReader;
}
}

void readPrimitives(CMesh& mesh, const CMaterial& material, atUint16 dataSize, Athena::io::BinaryReader& reader);

#endif // GXCOMMON_HPP
