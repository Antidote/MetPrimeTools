#ifndef MREADECOMPRESS_HPP
#define MREADECOMPRESS_HPP

#include <Athena/BinaryReader.hpp>
#include <Athena/FileWriter.hpp>

namespace RetroCommon
{

bool MREADecompress(Athena::io::BinaryReader& in, Athena::io::FileWriter& out);

}

#endif // MREADECOMPRESS_HPP
