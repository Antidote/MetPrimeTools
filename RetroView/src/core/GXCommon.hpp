#ifndef GXCOMMON_HPP
#define GXCOMMON_HPP

#include <Athena/Global.hpp>
#include "SBoundingBox.hpp"
#include "GXTypes.hpp"
#include "CColor.hpp"

#define KCOLORS "konst"

void drawWireSphere(float radius, atUint32 longs, atUint32 lats);
void drawSolidSphere(double radius, atUint32 slices, atUint32 stacks);
void drawOutlinedCube(const CVector3f& position, const CColor& mainColor, const CColor& outlineColor, const CVector3f& scale = CVector3f(1.0f));
void drawTexturedCube(atUint32 texture, const CVector3f& position, float alpha, const CVector3f& scale = CVector3f(1.0f));
void drawBoundingBox(SBoundingBox bbox, const CColor& borderColor = CColor(1.0, 0.0, 0.0));
void GXSetBlendMode(EBlendMode src, EBlendMode dst);
void drawCone(float width, float height, float offset);
void drawAxis(const CVector3f& translation, const CVector3f& orientation, float scale, bool disableDepth = true);
atUint64 assetIdFromPath(const std::string& filepath);

class CMesh;
class CModelData;
class CMaterial;

namespace Athena
{
namespace io
{
class IStreamReader;
}
}

void readPrimitives(CMesh& mesh, CModelData& model, const CMaterial& material, Athena::io::IStreamReader& reader);

long hiresTimeMS();
float hiresTimeSec();
float secondsMod900();

#endif // GXCOMMON_HPP
