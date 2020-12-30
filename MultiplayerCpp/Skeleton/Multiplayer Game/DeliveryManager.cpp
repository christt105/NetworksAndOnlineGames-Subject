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
	ret->delegate = del; // TODO

	pendingDeliveries.push_back(ret);

	++nextOutgoingSequenceNumber;

	return ret;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	uint32 seq = 0u;

	packet >> seq;

	if (seq < nextExpectedSequenceNumber) {
		return false;
	}

	pendingSequenceNumbers.push_back(seq);
	nextExpectedSequenceNumber = seq + 1;

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
				(*i)->delegate->onDeliverySuccess(this);
				pendingDeliveries.erase(i);
				delete* i;
				break;
			}
		}
	}
}

void DeliveryManager::processTimedoutPackets()
{
	for (auto item = pendingDeliveries.begin(); item != pendingDeliveries.end();) {
		if ((*item)->dispatchTime + 5 < Time.time) {
			if ((*item)->delegate != nullptr) {
				(*item)->delegate->onDeliveryFailure(this);
			}
			item = pendingDeliveries.erase(item);
		}
		else {
			++item;
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

void OnChangeNetworkIDDelegate::onDeliveryFailure(DeliveryManager* deliveryManager)
{
	if (client->connected) {
		OutputMemoryStream packet;
		packet << PROTOCOL_ID;
		client->delivery_manager.writeSequenceNumber(packet, new OnChangeNetworkIDDelegate(client));
		packet << ServerMessage::ChangeNetworkID;
		packet << client->gameObject->networkId;
		App->modNetServer->sendPacket(packet, client->address);
	}
}

void OnSendPendingAck::onDeliveryFailure(DeliveryManager* deliveryManager)
{
	OutputMemoryStream packet;
	packet << PROTOCOL_ID;
	packet << messageType;
	deliveryManager->writeSequenceNumber(packet);
	deliveryManager->writeSequenceNumbersPendingAck(packet);
	if (App->modNetClient != nullptr) {
		App->modNetClient->sendPacket(packet, addr);
	}
	else if (App->modNetServer != nullptr) {
		App->modNetServer->sendPacket(packet, addr);
	}
}
