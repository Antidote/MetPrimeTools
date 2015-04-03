#include "core/CResourceManager.hpp"

void IResource::destroy()
{
    CResourceManager::instance()->destroyResource(this);
}

