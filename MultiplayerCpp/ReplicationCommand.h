#pragma once

enum class ReplicationAction
{
    None, Create, Update, Destroy, PlayAudio
};

struct ReplicationCommand
{
    ReplicationAction action;
    uint32 networkId;
};