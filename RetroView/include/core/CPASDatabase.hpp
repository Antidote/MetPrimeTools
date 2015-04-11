#ifndef CPASDATABASE_HPP
#define CPASDATABASE_HPP

#include "core/CPASParameter.hpp"

#include <Athena/IStreamReader.hpp>
#include <vector>

struct SPASParmInfo
{
    EParmType type;
    atUint32 unknown1;
    float    unknown2;

    IPASParameterBase parameters[2];
};

struct SPASAnimInfo
{
    atUint32 unknown1;
    std::vector<IPASParameterBase> parameters;
};

struct SPASAnimState
{
    atUint32 unknown1;
    std::vector<SPASParmInfo> parmInfo;
    std::vector<SPASAnimInfo> animInfo;
};

class CPASDatabase
{
public:
    CPASDatabase();
    CPASDatabase(Athena::io::IStreamReader& in);
    ~CPASDatabase();
private:
    atInt32 m_defaultState;

    std::vector<SPASAnimState> m_states;
};

#endif // CPASDATABASE_HPP
