#ifndef CSCRIPTOBJECT_HPP
#define CSCRIPTOBJECT_HPP

#include "core/CProperty.hpp"

#include <vector>

class CScriptObject
{
public:
    CScriptObject();
    ~CScriptObject();
private:
    CStructProperty m_rootProperty;
};

#endif // CSCRIPTOBJECT_HPP
