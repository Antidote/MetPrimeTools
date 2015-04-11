#include "core/CPASDatabase.hpp"
#include <Athena/InvalidDataException.hpp>

CPASDatabase::CPASDatabase()
{
}

CPASDatabase::CPASDatabase(Athena::io::IStreamReader& in)
{
    atUint32 magic = in.readUint32();
    if (magic != 0x50415334)
        THROW_INVALID_DATA_EXCEPTION("Invalid PASDatabase magic expected 0x50415334 0x%X", magic);

    atUint32 stateCount = in.readUint32();
    m_defaultState = in.readInt32();
    if (m_defaultState == ~0)
        return;

    while ((stateCount--) > 0)
    {
        SPASAnimState state;
        state.unknown1 = in.readUint32();
        atUint32 parmInfoCount = in.readUint32();
        atUint32 animInfoCount = in.readUint32();

        for (atUint32 i = 0; i < parmInfoCount; i++)
        {
            SPASParmInfo info;
            info.type = (EParmType)in.readUint32();
            info.unknown1 = in.readUint32();
            info.unknown2 = in.readFloat();

            for (atUint32 j = 0; j < 2; j++)
            {
                switch(info.type)
                {
                case eInt32:
                {
                    atInt32 tmp = in.readInt32();
                    info.parameters[j] = CPASInt32Paramater(tmp);
                }
                    break;
                case eUint32:
                {
                    atUint32 tmp = in.readUint32();
                    info.parameters[j] = CPASUint32Parameter(tmp);
                }
                    break;
                case eEnum:
                {
                    atUint32 tmp = in.readUint32();
                    info.parameters[j] = CPASEnumParameter(tmp);
                }
                    break;
                case eReal32:
                {
                    float tmp = in.readFloat();
                    info.parameters[j] = CPASReal32Parameter(tmp);
                }
                    break;
                case eBool:
                {
                    bool tmp = in.readBool();
                    info.parameters[j] = CPASBoolParameter(tmp);
                }
                    break;
                }
            }
            state.parmInfo.push_back(info);
        }

        for (atUint32 i = 0; i < animInfoCount; i++)
        {
            SPASAnimInfo info;
            info.unknown1 = in.readUint32();
            for (atUint32 j = 0; j < parmInfoCount; j++)
            {
                switch(state.parmInfo[j].type)
                {
                case eInt32:
                {
                    atInt32 tmp = in.readInt32();
                    info.parameters.push_back(CPASInt32Paramater(tmp));
                }
                    break;
                case eUint32:
                {
                    atUint32 tmp = in.readUint32();
                    info.parameters.push_back(CPASUint32Parameter(tmp));
                }
                    break;
                case eEnum:
                {
                    atUint32 tmp = in.readUint32();
                    info.parameters.push_back(CPASEnumParameter(tmp));
                }
                    break;
                case eReal32:
                {
                    float tmp = in.readFloat();
                    info.parameters.push_back(CPASReal32Parameter(tmp));
                }
                    break;
                case eBool:
                {
                    bool tmp = in.readBool();
                    info.parameters.push_back(CPASBoolParameter(tmp));
                }
                    break;
                }
            }
            state.animInfo.push_back(info);
        }
        m_states.push_back(state);
    }
}

CPASDatabase::~CPASDatabase()
{
}
