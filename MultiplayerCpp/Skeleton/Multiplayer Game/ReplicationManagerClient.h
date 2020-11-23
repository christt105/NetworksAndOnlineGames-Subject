#pragma once
#include "MemoryStream.h"

class ReplicationManagerClient
{
public:
    void read(const InputMemoryStream& packet);
};