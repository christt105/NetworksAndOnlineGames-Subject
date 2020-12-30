#include "Networks.h"
#include "ModuleNetworkingServer.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream& packet, DeliveryDelegate* del)
{
	packet << nextOutgoingSequenceNumber;

	Delivery* ret = new Delivery();
	ret->sequenceNumber = nextOutgoingSequenceNumber;
	ret->dispatchTime = Time.time;
	ret->delegate = del;

	pendingDeliveries.push_back(ret);

	++nextOutgoingSequenceNumber;

	return ret;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	uint32 seq = 0u;

	packet >> seq;

	if (seq != nextExpectedSequenceNumber) {
		return false;
	}

	pendingSequenceNumbers.push_back(seq);
	++nextExpectedSequenceNumber;

	return true;
}

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	return !pendingSequenceNumbers.empty();
}

const std::list<uint32>& DeliveryManager::getPendingAck() const
{
	return pendingSequenceNumbers;
}

void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream& packet)
{
	for (auto item = pendingSequenceNumbers.begin(); item != pendingSequenceNumbers.end(); ++item) {
		packet << *item;
	}
	pendingSequenceNumbers.clear();
}

void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream& packet)
{
	while (packet.RemainingByteCount() > 0) {
		uint32 seq = 0U;
		packet >> seq;

		for (auto i = pendingDeliveries.begin(); i != pendingDeliveries.end(); ++i) {
			if ((*i)->sequenceNumber == seq) {
				if ((*i)->delegate != nullptr) {
					(*i)->delegate->onDeliverySuccess(this);
				}
				delete* i;
				pendingDeliveries.erase(i);
				break;
			}
		}
	}
}

void DeliveryManager::processTimedoutPackets()
{
	for (auto item = pendingDeliveries.begin(); item != pendingDeliveries.end(); ++item) {
		if ((*item)->dispatchTime + PACKET_DELIVERY_TIMEOUT_SECONDS < Time.time) {
			if ((*item)->delegate != nullptr) {
				(*item)->delegate->onDeliveryFailure(this, *item);
			}
			(*item)->dispatchTime = Time.time;
		}
	}
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

void Delivery::CopyPacket(const OutputMemoryStream& packet)
{
	this->packet = packet;
}

void DeliveryDelegate::onDeliveryFailure(DeliveryManager* deliveryManager, Delivery* del)
{
	if (isServer) {
		App->modNetServer->sendPacket(del->packet, addr);
	}
	else {
		App->modNetClient->sendPacket(del->packet, addr);
	}
}
