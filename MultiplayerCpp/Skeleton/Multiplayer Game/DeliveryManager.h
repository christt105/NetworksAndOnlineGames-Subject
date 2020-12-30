#pragma once
#include <list>
// TODO(you): Reliability on top of UDP lab session

class DeliveryManager;
struct ClientProxy;

class DeliveryDelegate {
public:
	virtual ~DeliveryDelegate() {}
	virtual void onDeliverySuccess(DeliveryManager* deliveryManager) = 0;
	virtual void onDeliveryFailure(DeliveryManager* deliveryManager) = 0;
};

class OnChangeNetworkIDDelegate : public DeliveryDelegate {
public:
	OnChangeNetworkIDDelegate(ClientProxy* client) { this->client = client; }
	void onDeliverySuccess(DeliveryManager* deliveryManager) override {}
	void onDeliveryFailure(DeliveryManager* deliveryManager) override;
private:
	ClientProxy* client = nullptr;
};


class WelcomeDelegate : public DeliveryDelegate {
public:
	WelcomeDelegate(ClientProxy* client) { this->client = client; }
	void onDeliverySuccess(DeliveryManager* deliveryManager) override {}
	void onDeliveryFailure(DeliveryManager* deliveryManager) override;
private:
	ClientProxy* client = nullptr;
};

struct Delivery {
	~Delivery() { if (delegate != nullptr) { delete delegate; } }
	uint32 sequenceNumber = 0;
	double dispatchTime = 0.0;
	DeliveryDelegate* delegate = nullptr;
};

class DeliveryManager {
public:
	DeliveryManager() {}

	//For senders to write a new seq. numbers into a packet
	Delivery* writeSequenceNumber(OutputMemoryStream& packet, DeliveryDelegate* del = nullptr);

	//For receivers to process the seq. number from an incoming packet
	bool processSequenceNumber(const InputMemoryStream& packet);
	
	//For receivers to write ack'ed seq. numbers into a packet
	bool hasSequenceNumbersPendingAck() const;
	void writeSequenceNumbersPendingAck(OutputMemoryStream& packet);

	//For senders to process ack'ed seq. numbers from a packet
	void processAckdSequenceNumbers(const InputMemoryStream& packet);
	void processTimedoutPackets();

	void clear();

private:

	// Private members (sender side)
	// - The next outgoing sequence number
	// - A list of pending deliveries
	uint32 nextOutgoingSequenceNumber = 0u;
	std::list<Delivery*> pendingDeliveries;

	// Private members (receiver side)
	// - The next expected sequence number
	// - A list of sequence numbers pending ack
	uint32 nextExpectedSequenceNumber = 0u;
	std::list<uint32> pendingSequenceNumbers;
};