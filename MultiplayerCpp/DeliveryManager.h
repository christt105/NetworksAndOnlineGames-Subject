#pragma once
#include <list>
// TODO(you): Reliability on top of UDP lab session

#define TIME_OUT_PACKET 0.075

class DeliveryManager;
struct ClientProxy;
struct Delivery;

class DeliveryDelegate {
public:
	DeliveryDelegate(sockaddr_in addr, bool isServer) : addr(addr), isServer(isServer) {};
	virtual ~DeliveryDelegate() {}
	virtual void onDeliverySuccess(DeliveryManager* deliveryManager) {};
	virtual void onDeliveryFailure(DeliveryManager* deliveryManager, Delivery* delivery);

public:
	bool isServer;
	sockaddr_in addr;
};

struct Delivery {
	~Delivery() { if (delegate != nullptr) { delete delegate; } }
	void CopyPacket(const OutputMemoryStream& packet);
	uint32 sequenceNumber = 0;
	double dispatchTime = 0.0;
	DeliveryDelegate* delegate = nullptr;
	OutputMemoryStream packet;
};

class DeliveryManager {
public:
	DeliveryManager() {}

	//For senders to write a new seq. numbers into a packet
	Delivery* writeSequenceNumber(OutputMemoryStream& packet, DeliveryDelegate* del);

	//For receivers to process the seq. number from an incoming packet
	bool processSequenceNumber(const InputMemoryStream& packet);
	
	//For receivers to write ack'ed seq. numbers into a packet
	bool hasSequenceNumbersPendingAck() const;
	const std::list<uint32>& getPendingAck() const;
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