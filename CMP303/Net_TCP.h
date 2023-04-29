#pragma once
#ifndef NET_TCP_H
#define NET_TCP_H


#include "Net_Parent.h"
#pragma region GLOBAL_COMMANDS
static const UINT8 COM_CLIENTCALL = 100;

static const UINT8 COM_EGAMEFIREMODE = 182;
static const UINT8 COM_EGAMEDAMAGEENEMY = 183;
static const UINT8 COM_EGAMEENEMYCONSISTANCY = 184;
#pragma endregion
#include <queue>

class Net_TCP : public Net_Parent
{
public:
	Net_TCP();
	~Net_TCP();

	void StartListener();
	void S_RecievingClients();
	bool C_ConnectToHost(std::string ip, std::string port);
	void SetHost(bool host) { hosting = host; }
	bool IsHost() { return hosting; }
	static Net_TCP* GetInstance() { return instance; };
	//std::string playerName;
	void SetName(const char* name) { username = name; }
	void SetName(std::string name) { username = name; }
	std::string GetName() { return username; };
	int GetClientCount() { return clients.size(); }
	Net_UserInformation& GetClient(int i) { return clients[i]; }
	unsigned int udpPort;

	void TCP_Init();
	void S_NewConnection();

	void A_WaitForMessage();
	void A_CheckCommand(sf::Packet packet);

	void C_AddClients(sf::Packet packet);
	void C_SendPresenceConfirm();
	void S_CheckPresenceConfirm(sf::Packet packet);

	void A_PresenceCheck(float time_elapsed);

	void C_LeaveLobby();
	void S_CloseLobby();

	void S_TransferHost();
	void C_RecieveHostTransfer(sf::Packet packet);
	void S_Designate2IC();
	void C_Ack2IC(sf::Packet packet);

	void RemoveClient(sf::Packet packet);
	void KickClient(int clientNumber);
	void Disconnect();

	void StartGame();
	bool startGame = false;
	bool forceLeave = false;


	void InitGame();

	void UpdateState(Net_UserState newState);
	void UpdateState(Net_UserState newState, int identifier);
	void InterpretUpdate(sf::Packet packet);

	//Generic message function
	void SendMessage(sf::Packet message);
	void SendMessage(sf::Packet message, int identity);//The client to omit
	std::queue<UINT8> eventCommands;//This will tell us what event has triggered
	std::queue<int> eventIndicesInformation;//This will allow us to grab any necessary information from the below queues, in the correct order.
	/*
	* The system for the event system goes as such:
	* 
	* use eventCommands to find out what function to execute
	* while eventCommands is not empty
	*	while eventIndices is not empty or eventIndices != -1
	*		int dataType = first element of eventIndices
	*		if(dataType == 1)
	*			grab first element of eventIntInformation and pop
	*		if(dataType == 2) 
	*			grab first element of eventFloatInformation and pop
	* 
	*		pop last element from eventIndices
	*

	This system only needs to be utilised if there is a rather complex event. May not even need to use this in the end

	This system does not currently need to be used as usually only one event will ever be sent at a single time
	//Note: Consider Mutex locking as there could be an issue with requesting the same information on both threads
	*/
	std::queue<UINT8> eventIntInformation;

	bool secondCommand = false;
private:
	std::string A_GetClientUsername(sf::TcpSocket* connectedSocket)
	{
		sf::Packet recvPack;

		connectedSocket->receive(recvPack);

		std::string name;

		if (recvPack >> name)
		{
			return name;
		}
		return "#";//Users are not allowed to type #
	}

	void A_SendUsername(sf::TcpSocket* connectedSocket)
	{
		sf::Packet sendPack;

		sendPack << username;

		connectedSocket->send(sendPack);
	}

	static Net_TCP* instance;
	sf::Socket::Status sockStatus;//Error Codes

	bool hosting;
	std::string username;


	std::string text = "Connect to: ";
	std::size_t recieved;
	char buffer[200];
	bool sending;


	/// <summary>
	/// [0] will always be self
	/// [1] will be host(for client side)
	/// Host will have information regarding ip, port, socket etc whereas clients will only have the username of other clients. Everyone knows the hosts ip, respective ports and sockets connected
	/// 2ic will have the same information as host.
	/// If host disconnects. 2IC will become host by:
	/// Sending all clients a newhost command. Clients will then disconnect from host and reconnect to 2ic. 
	/// When successful comms are established between 2ic and clients, the process is complete and the game continues
	/// The now host, will designate another 2ic
	/// </summary>
	std::vector<Net_UserInformation> clients;//Only relevant to Host for using this to store all clients
	//std::vector<sf::TcpSocket*> clients;
	sf::SocketSelector selector;

	int timeoutTime = 5;
};



#endif // !NET_TCP_H
