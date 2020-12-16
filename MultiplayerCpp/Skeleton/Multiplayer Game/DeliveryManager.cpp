#include "Networks.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream& packet)
{
	packet << nextOutgoingSequenceNumber;

	Delivery* ret = new Delivery();
	ret->sequenceNumber = nextOutgoingSequenceNumber;
	ret->dispatchTime = Time.time;
	ret->delegate = nullptr; // TODO

	pendingDeliveries.push_back(ret);

	return ret;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	uint32 seq = 0u;

	packet >> seq;

	return false;
}

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	return !pendingSequenceNumbers.empty();
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
	nextOutgoingSequenceNumber = 0U;
	for (auto i = pendingDeliveries.begin(); i != pendingDeliveries.end(); ++i) {
		delete* i;
	}
	pendingDeliveries.clear();
	
	nextExpectedSequenceNumber = 0U;
	pendingSequenceNumbers.clear();
}
