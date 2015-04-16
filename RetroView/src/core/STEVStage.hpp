#ifndef TEVSTAGE_HPP
#define TEVSTAGE_HPP

#include <Athena/Types.hpp>
#include <memory.h>
#include <string>
#include <QString>

struct STEVStage final
{
    STEVStage();
    atUint32 ColorInFlags;
    atUint32 AlphaInFlags;
    atUint32 ColorOpFlags;
    atUint32 AlphaOpFlags;
    atUint8  Padding;
    atUint8  KonstAlphaIn;
    atUint8  KonstColorIn;
    atUint8  RasterizedIn;
    QStringList fragmentSource(atUint32 texTEVIn, atUint32 textureCount, atUint32 idx);
};

bool operator==(const STEVStage& left, const STEVStage& right);

#endif // TEVSTAGE_HPP
