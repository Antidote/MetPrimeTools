#ifndef MREADECOMPRESS_HPP
#define MREADECOMPRESS_HPP

#include <Athena/BinaryReader.hpp>
#include <Athena/BinaryWriter.hpp>

namespace RetroCommon
{

bool MREADecompress(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);

}

#endif // MREADECOMPRESS_HPP
