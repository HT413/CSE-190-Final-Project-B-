#include "ClientGame.h"
#include "Project4.h"
#include <sstream>

ClientGame::ClientGame()
{
	network = new ClientNetwork();

	// send init packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_CONNECTION;

	packet.serialize(packet_data);

	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}


ClientGame::~ClientGame()
{
}

void ClientGame::sendActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::update()
{
	Packet packet;
	int data_length = network->receivePackets(network_data);

	if(data_length <= 0)
	{
		//no data recieved
		return;
	}

	int i = 0;
	while(i < (unsigned int)data_length)
	{
		packet.deserialize(&(network_data[i]));
		i += sizeof(Packet);

	
		switch(packet.packet_type) {
		case RIFT_HAND_LOC: 
		{
			/* data structures to split string by ',' */
			std::stringstream ss;
			std::vector<std::string> handPosValues;
			std::string split;


			char loc[32 * sizeof(Packet)];
			for (int j = 0; j < 32 * sizeof(Packet); j++) {
				packet.deserialize(&(network_data[i + j]));
			}
			memcpy(loc, network_data + i, 32 * sizeof(Packet));

			ss.str(loc);
			while (std::getline(ss, split, ',')) {
				//split contains the coorindates of hand position
				if (split.length() > 2) {	//ignore the first nonsense characters
					handPosValues.push_back(split);
				}
			}

			vec3 theHandPosition; 
			theHandPosition.x = std::stof(handPosValues[0]);	//stof converts string to float
			theHandPosition.y = std::stof(handPosValues[1]);
			theHandPosition.z = std::stof(handPosValues[2]);
			cout << theHandPosition.x  << " " << theHandPosition.y << " " <<theHandPosition.z<<endl;

			i += 32 * sizeof(Packet);
			break;
		}
		case GAME_START_NOTICE:
			gameStart = !gameStart;
			printf("Opponent found! Game now starting!\n");
			break;

		case ACTION_EVENT:
			printf("Server ping was successful!\n");
			//sendActionPackets();
			break;

		default:
			//printf("Error in packet types\n");
			break;
		}
	}
}