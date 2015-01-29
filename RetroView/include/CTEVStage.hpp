#ifndef TEVSTAGE_HPP
#define TEVSTAGE_HPP

#include <Athena/Types.hpp>
#include <string>

struct STEVStage
{
    atUint32 ColorInFlags;
    atUint32 AlphaInFlags;
    atUint32 ColorOpFlags;
    atUint32 AlphaOpFlags;
    atUint8  Padding;
    atUint8  KonstAlphaIn;
    atUint8  KonstColorIn;
    atUint8  RasterizedIn;
};

#endif // TEVSTAGE_HPP
