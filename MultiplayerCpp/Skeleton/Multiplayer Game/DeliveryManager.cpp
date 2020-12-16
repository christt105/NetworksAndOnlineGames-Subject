#include "Networks.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream& packet)
{
	return nullptr;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	return false;
}

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	return false;
}

void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream& packet)
{
}

void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream& packet)
{
}

void DeliveryManager::processTimedoutPackets()
{
}

void DeliveryManager::clear()
{
}
