#ifndef CPASPARAMETER
#define CPASPARAMETER

#include <Athena/Global.hpp>

enum EParmType
{
    eInt32,
    eUint32,
    eReal32,
    eBool,
    eEnum
};
class IPASParameterBase
{
};

template <typename T>
class IPASParameter : public IPASParameterBase
{
public:
    IPASParameter(const T& val)
        : m_value(val)
    {}

    T value() const { return m_value; }

    inline void operator =(const T& val)
    {
        m_value = val;
    }

    inline bool operator ==(const T& val)
    {
        return m_value == val;
    }

    inline bool operator !=(const T& val)
    {
        return !(*this == val);
    }

    inline bool operator >(const T& val)
    {
        return m_value > val;
    }

    inline bool operator >=(const T& val)
    {
        return m_value >= val;
    }

    inline bool operator <(const T& val)
    {
        return m_value < val;
    }

    inline bool operator <=(const T& val)
    {
        return m_value <= val;
    }

private:
    T m_value;
};


typedef IPASParameter<float>    CPASReal32Parameter;
typedef IPASParameter<atInt32>  CPASInt32Paramater;
typedef IPASParameter<atUint32> CPASUint32Parameter;
typedef CPASUint32Parameter     CPASEnumParameter;
typedef IPASParameter<bool>     CPASBoolParameter;


#endif // CPASPARAMETER

