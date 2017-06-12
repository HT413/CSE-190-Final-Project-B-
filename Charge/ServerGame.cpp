#include "ServerGame.h"
#include "Project4.h"
#include <sstream>

unsigned int ServerGame::client_id;

ServerGame::ServerGame()
{
	// id's to assign clients for our table
	client_id = 0;

	// set up the server network to listen 
	network = new ServerNetwork();
}

ServerGame::~ServerGame()
{
}

void ServerGame::update()
{
	// get new clients
	if (network->acceptNewClient(client_id))
	{
		printf("client %d has been connected to the server\n", client_id);
		client_id++;
		// Inform the clients that the game should now start
		if (client_id == 2) {
			notifyStart();
		}
	}

	receiveFromClients();
}

void ServerGame::receiveFromClients()
{
	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;
	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		int data_length = network->receiveData(iter->first, network_data);
		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}
		int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type) {
			case RIFT_UNIT_CREATION:
			{
				std::stringstream ss;
				std::vector<std::string> unitValues;
				std::string split;



				char unitInfo[32 * sizeof(Packet)];
				for (int j = 0; j < 32 * sizeof(Packet); j++) {
					packet.deserialize(&(network_data[i + j]));
				}

				memcpy(unitInfo, network_data + i, 32 * sizeof(Packet));

				ss.str(unitInfo);
				while (std::getline(ss, split, ',')) {
					//split contains the coorindates of hand position
					if (split.length() > 1) {	//ignore the first nonsense characters
						unitValues.push_back(split);
					}
				}



				if (unitValues.size() > 1) {
					ACTOR_TYPE unitType = static_cast<ACTOR_TYPE>(int(stof(unitValues[1])));
					int unitID = int(stof(unitValues[2]));
					sentRiftUnitCreation((float)unitType + 0.1f, (float)unitID - 0.1f);
				}

				i += 32 * sizeof(Packet);
				
				break;
			}

			case LEAP_UNIT_CREATION:
			{
				std::stringstream ss;
				std::vector<std::string> unitValues;
				std::string split;

		

				char unitInfo[32 * sizeof(Packet)];
				for (int j = 0; j < 32 * sizeof(Packet); j++) {
					packet.deserialize(&(network_data[i + j]));
				}
				
				memcpy(unitInfo, network_data + i, 32 * sizeof(Packet));
			
				ss.str(unitInfo);
				while (std::getline(ss, split, ',')) {
					//split contains the coorindates of hand position
					if (split.length() > 1) {	//ignore the first nonsense characters
						unitValues.push_back(split);
					}
				}
			


				if (unitValues.size() > 1) {
					ACTOR_TYPE unitType = static_cast<ACTOR_TYPE>(int(stof(unitValues[1])));
					int unitID = int(stof(unitValues[2]));
					sentLeapUnitCreation((float)unitType + 0.1f, (float)unitID - 0.1f);
				}

				i += 32 * sizeof(Packet);
				break;
			}

			case LEAP_HAND_LOC:
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
				if (handPosValues.size() > 2) {
					theHandPosition.x = std::stof(handPosValues[0]);	//stof converts string to float
					theHandPosition.y = std::stof(handPosValues[1]);
					theHandPosition.z = std::stof(handPosValues[2]);
					//cout << "Received leap hand pos: " << theHandPosition.x << ", " << theHandPosition.y << ", " << theHandPosition.z << endl;
					updateLeapPos(vec3(theHandPosition.x, theHandPosition.y, theHandPosition.z));
				}

				i += 32 * sizeof(Packet);

				break;
			}

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
					if (handPosValues.size() > 2) {
						theHandPosition.x = std::stof(handPosValues[0]);	//stof converts string to float
						theHandPosition.y = std::stof(handPosValues[1]);
						theHandPosition.z = std::stof(handPosValues[2]);
					}

					i += 32 * sizeof(Packet);
					sendRiftHandPos(theHandPosition.x, theHandPosition.y, theHandPosition.z);
					break;
				}

			case INIT_CONNECTION:
				//printf("server received init packet from client\n");
				//sendActionPackets();
				break;

			case ACTION_EVENT:
				//printf("server received action event packet from client\n");
				//sendActionPackets();
				break;

			default:
				//printf("error in packet types\n");
				break;
			}
		}
	}
}


void ServerGame::sendActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}

void ServerGame::notifyStart() {
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = GAME_START_NOTICE;

	packet.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}

void ServerGame::sendRiftHandPos(float x, float y, float z) {
	const unsigned int packet_size = 33 * sizeof(Packet);
	std::ostringstream ss;
	ss << "ZZ" << "," << x << "," << y << "," << z;
	const char * str = ss.str().c_str();
	char packet_data[packet_size];
	strcpy(packet_data + 4, str);

	Packet packet;
	packet.packet_type = RIFT_HAND_LOC;

	packet.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}

void ServerGame::sentRiftUnitCreation(float type, float id) {
	const unsigned int packet_size = 33 * sizeof(Packet);
	std::ostringstream ss;
	ss << "ZZ" << "," << type << "," << id;

	char* cstr = new char[packet_size];
	std::strcpy(cstr, ss.str().c_str());

	char packet_data[packet_size];
	strcpy(packet_data + 4, cstr);

	Packet packet;
	packet.packet_type = RIFT_UNIT_CREATION;

	packet.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}


void ServerGame::sentLeapUnitCreation(float type, float id) {
	const unsigned int packet_size = 33 * sizeof(Packet);
	std::ostringstream ss;
	ss << "ZZ" << "," << type << "," << id;

	char* cstr = new char[packet_size];
	std::strcpy(cstr, ss.str().c_str());

	char packet_data[packet_size];
	strcpy(packet_data + 4, cstr);

	Packet packet;
	packet.packet_type = LEAP_UNIT_CREATION;

	packet.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}