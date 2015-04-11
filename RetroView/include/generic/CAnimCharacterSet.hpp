#ifndef CANIMCHARACTERSET_HPP
#define CANIMCHARACTERSET_HPP

#include "core/IResource.hpp"
#include "generic/CAnimCharacterNode.hpp"

class CAnimCharacterSet : public IResource
{
public:
    enum Version
    {
        MetroidPrime1,
        MetroidPrime2
    };

    CAnimCharacterSet();
    ~CAnimCharacterSet();

    CAnimCharacterNode* nodeByName(const std::string& name);
    CAnimCharacterNode* nodeByIndex(const atUint32 idx);
private:
    friend class CAnimCharacterSetReader;
    std::vector<CAnimCharacterNode*> m_characterNodes;
    Version m_version;
};

#endif // CANIMCHARACTERSET_HPP
