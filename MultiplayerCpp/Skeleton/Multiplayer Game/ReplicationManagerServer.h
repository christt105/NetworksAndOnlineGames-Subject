#pragma once

#include <map>
#include "MemoryStream.h"

enum class ReplicationAction;

class ReplicationManagerServer
{
public:
    void create(uint32 networkId);
    void update(uint32 networkId);
    void destroy(uint32 networkId);

    void write(OutputMemoryStream& packet);
public:

private:

    std::map<uint32, ReplicationAction> actions;

};