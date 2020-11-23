#pragma once

enum class ReplicationAction
{
    None, Create, Update, Destroy
};

struct ReplicationCommand
{
    ReplicationAction action;
    uint32 networkId;
};