#include "core/STEVStage.hpp"

bool operator==(const STEVStage& left, const STEVStage& right)
{
    return !memcmp(&left, &right, sizeof(STEVStage));
}
