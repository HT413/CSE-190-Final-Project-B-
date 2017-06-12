#ifndef _NETWORK_COMM_GAME_SERVER_
#define _NETWORK_COMM_GAME_SERVER_

#include "ServerNetwork.h"
#include "NetworkData.h"

extern enum ACTOR_TYPE;

class ServerGame
{
public:
	ServerGame();
	~ServerGame();

	void update();
	void receiveFromClients();
	void sendActionPackets();
	void notifyStart();
	void sendRiftHandPos(float, float, float);

private:
	// IDs for the clients connecting for table in ServerNetwork 
	static unsigned int client_id;

	// The ServerNetwork object 
	ServerNetwork* network;

	// data buffer
	char network_data[MAX_PACKET_SIZE];
};

#endif