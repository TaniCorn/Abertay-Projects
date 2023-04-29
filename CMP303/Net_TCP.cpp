#include "Net_TCP.h"
#include <iostream>

#ifdef NDEBUG
#define DEBUG(x) 
#else
#define DEBUG(x) do { std::cout << x << std::endl; } while (0)
#endif

Net_TCP* Net_TCP::instance = nullptr;
sf::TcpSocket* socket = new sf::TcpSocket();//Only relevant to client for socket connection to host and hosts temp client data
sf::TcpListener listener;//Only relevant to host to listen for new clients
static int presenceCounter[3] = { 0,0,0 };
#pragma region LOCAL_COMMANDS

static const UINT8 COM_ADDCLIENT = 150;
static const UINT8 COM_REMOVECLIENT = 151;
static const UINT8 COM_PRESENCE = 152;
static const UINT8 COM_PRESENCECONFIRM = 153;
static const UINT8 COM_LEAVING = 154;
static const UINT8 COM_CLOSING = 155;
static const UINT8 COM_START = 156;
static const UINT8 COM_INITGAME = 157;
static const UINT8 COM_GAMEUPDATE = 180;
static const UINT8 COM_GAMEPOSITION = 181;
static const UINT8 COM_NEWHOST = 200;
static const UINT8 COM_2IC = 201;

#pragma endregion
Net_TCP::Net_TCP() :
	buffer("NULL"),
	hosting(1),
	sending(1),
	recieved(777),
	clients(NULL)
{
	instance = this;

	std::cout << "TCP: Initialising\n";
	//Binding our socket
	//Remember 127.0.0.1 is the local machine address
	std::cout << "TCP: Your IP address is: " << sf::IpAddress::getPublicAddress().toString() << std::endl;

	/*sockStatus = listener.listen(sf::TcpListener::AnyPort);
	if (sockStatus == sf::Socket::Error) {
		std::cout << "TCP:Listener Failed : ";
		std::cout << "TCP HAS FAILED";
		delete instance;
		return;
	}
	std::cout << "TCP:Your Socket port is:" << listener.getLocalPort() << "\n";*/

	Net_UserInformation self;
	self.ipAddress = sf::IpAddress::getPublicAddress().toString();
	self.port = socket->getLocalPort();
	self.socket = socket;
	self.username = "DefaultName";
	clients.push_back(self);


	socket->setBlocking(true);
	listener.setBlocking(false);

}

Net_TCP::~Net_TCP()
{
	selector.clear();
	for (std::vector<Net_UserInformation>::iterator it = clients.begin(); it != clients.end(); it++)
		delete it->socket;
}

void Net_TCP::TCP_Init() {

}
#pragma region JOINING_FUNCTIONS
void Net_TCP::StartListener() {
	listener.setBlocking(false);
	listener.listen(sf::TcpListener::AnyPort);
	std::cout << "TCP:Your Socket port is:" << listener.getLocalPort() << "\n";

}
void Net_TCP::S_RecievingClients()
{
	if (hosting != HOST)
		return;

	//When we accept a connection, we will start a new listener and a new socket for the connection.
	sockStatus = listener.accept(*socket);
	if (sockStatus == sf::Socket::Done)
	{
		std::cout << "TCP:Connection successfully established!";
		
		S_NewConnection();

		socket = nullptr;
		socket = new sf::TcpSocket();
		StartListener();
	}

}
void Net_TCP::S_NewConnection() {
	//Fill in client information
	Net_UserInformation client;
	client.socket = socket;
	client.port = socket->getRemotePort();
	client.ipAddress = socket->getRemoteAddress().toString();
	client.username = A_GetClientUsername(socket);
	A_SendUsername(socket);

	clients.push_back(client);
	selector.add(*socket);

	//Informs all current clients of new client
	for (int i = 1; i < clients.size() - 1; i++)
	{
		sf::Packet newClient;
		newClient << UINT8(COM_ADDCLIENT) << client.username;
		clients[i].socket->send(newClient);
	}

	//Informs new client of all current clients
	sf::Packet ncPack;
	ncPack << UINT8(COM_ADDCLIENT);
	for (int i = 1; i < clients.size() - 1; i++)
	{
		ncPack << clients[i].username;
	}
	socket->send(ncPack);



}
bool Net_TCP::C_ConnectToHost(std::string ip, std::string port)
{
	if (hosting != CLIENT)
		return false;

	listener.close();
	UINT16 serverPort = std::stoi(port);
	sf::IpAddress serverIP(ip);
	sockStatus = socket->connect(serverIP, serverPort);
	if (sockStatus == sf::Socket::Error)
	{
		std::cout << "TCP:Error trying to establish connection";
		return false;
	}
	if (sockStatus == sf::Socket::NotReady)
	{
		std::cout << "TCP:Wrong port/Socket is not ready to recieve";
		return false;
	}
	if (sockStatus == sf::Socket::Done)
	{
		sending = 0;
		//Send host our username
		A_SendUsername(socket);

		//Add hosts info to our clients
		Net_UserInformation client;
		client.socket = socket;
		client.port = socket->getRemotePort();
		client.ipAddress = socket->getRemoteAddress().toString();
		client.username = A_GetClientUsername(socket);

		clients.push_back(client);
		selector.add(*socket);


		return true;
	}
	return false;
}


void Net_TCP::A_WaitForMessage() {

	//This loop will simply wait to recieve a command function, then execute a function with the rest of the message parameters
	if (selector.wait(sf::milliseconds(100)))
	{
		//std::cout << "A_WaitForMessage: Selector Wait Ready" << std::endl;

		for (int i = 1; i < clients.size(); i++)
		{
			if (clients[i].socket == nullptr)//Clients have nullptr socket references for other clients.
				continue;

			if (selector.isReady(*clients[i].socket))
			{
				sf::Packet recvPack;
				sockStatus = clients[i].socket->receive(recvPack);

				if (sockStatus == sf::Socket::Done)
					A_CheckCommand(recvPack);
				else if (sockStatus == sf::Socket::Partial)
					std::cout << "A_WaitForMessage: Recieved a partial packet" << std::endl;//We will not be handling partial packets
				//else
					//std::cout << "A_WaitForMessage: Unaccounted Socket Status" << std::endl;

			}

		}

	}

}
void Net_TCP::A_CheckCommand(sf::Packet packet) {

	UINT8 command;
	if (packet >> command) {
		switch (command)
		{
		case UINT8(COM_ADDCLIENT):
			C_AddClients(packet);
			break;
		case UINT8(COM_PRESENCE):
			C_SendPresenceConfirm();
			break;
		case UINT8(COM_PRESENCECONFIRM):
			S_CheckPresenceConfirm(packet);
			break;
		case UINT8(COM_LEAVING):
			RemoveClient(packet);
			break;
		case UINT8(COM_CLOSING):
			C_LeaveLobby();
			break;
		case UINT8(COM_2IC):
			C_Ack2IC(packet);
			break;
		case UINT8(COM_START):
			int seed;
			packet >> seed;
			eventIndicesInformation.push(seed);
			InitGame();
			startGame = true;
			break;
		case UINT8(COM_GAMEUPDATE):
			InterpretUpdate(packet);
			break;
		case UINT8(COM_EGAMEFIREMODE):
			eventCommands.push(COM_EGAMEFIREMODE);
			for (UINT8 i = 0; i < 2; i++)//first one will be playerID, second will be firetype
			{
				UINT8 info;
				packet >> info;
				eventIntInformation.push(info);
			}

			break;
		case UINT8(COM_EGAMEDAMAGEENEMY):
			eventCommands.push(COM_EGAMEDAMAGEENEMY);
			UINT8 amount;
			packet >> amount;
			eventIntInformation.push(amount);
			for (UINT8 i = 0; i < amount; i++)
			{
				UINT8 info;
				packet >> info;
				eventIntInformation.push(info);
			}
			break;
		case UINT8(COM_EGAMEENEMYCONSISTANCY):
			eventCommands.push(COM_EGAMEENEMYCONSISTANCY);
			UINT8 amount2;
			packet >> amount2;
			eventIntInformation.push(amount2);
			for (UINT8 i = 0; i < amount2; i += 2)
			{
				UINT8 enemyId;
				UINT8 clientId;
				packet >> enemyId >> clientId;
				eventIntInformation.push(enemyId);
				eventIntInformation.push(clientId);
			}

			break;
		case UINT8(COM_CLIENTCALL):
			UINT8 identity;
			packet >> identity;
			SendMessage(packet, identity);
			A_CheckCommand(packet);
			return;
		default:
			std::cout << "CheckCommand: Recieved unknown command" << std::endl;
			//#if _DEBUG
			////This is just if we need to debug a weird message
			//std::vector<std::string> debug;
			//std::string str = "Empty";
			//while (packet >> str)
			//{
			//	debug.push_back(str);
			//}
			//#endif
			break;
		}
	}


}


void Net_TCP::C_AddClients(sf::Packet packet) {
	if (hosting == HOST)
		return;
	//Adds clients (Only adds username and creates fake info for everything else)
	std::string name;

	while (packet >> name) {
		Net_UserInformation newClient;
		newClient.username = name;
		newClient.ipAddress = "NULL";
		newClient.port = 0;
		newClient.socket = nullptr;
		clients.push_back(newClient);
	}

}

void Net_TCP::C_LeaveLobby() {
	//Sends our IP and port number
	//Currently the port number is to allow a local connection, we don't do usernames since 2 people can have the same name.
	//It is just an extra step of verification so we don't remove the wrong person
	sf::Packet leavePack;
	leavePack << COM_LEAVING << ip.toString() << clients[1].socket->getLocalPort();
	socket->send(leavePack);
	Disconnect();
	
}
void Net_TCP::S_CloseLobby() {
	//Tell all clients to leave
	sf::Packet closingPack;
	closingPack << COM_CLOSING;
	for (int i = 1; i < clients.size(); i++)
	{
		sockStatus = clients[i].socket->send(closingPack);
	}

	//Disconnect the sockets, clear selector and clean everything TCP wise.
	for (int i = 1; i < clients.size(); i++)
	{
		clients[i].socket->disconnect();
	}
	selector.clear();
	while (clients.size() > 1)
	{
		clients.pop_back();
	}
}
#pragma region MIGRATION
void Net_TCP::S_TransferHost()
{

	sf::Packet newHost;
	newHost << COM_NEWHOST;
	for (int i = 1; i < clients.size(); i++)
	{
		newHost << clients[i].ipAddress << clients[i].port << clients[i].username << clients[i].state;
	}

	clients[1].socket->send(newHost);

}
void Net_TCP::C_RecieveHostTransfer(sf::Packet packet)
{
}
void Net_TCP::S_Designate2IC()
{
	//Sends the first client a list of all other client info
	sf::Packet second;
	second << COM_2IC;
	second << (clients.size() - 2);
	for (int i = 2; i < clients.size(); i++)
	{
		second << clients[i].ipAddress << clients[i].port << clients[i].username << clients[i].state << clients[i].identifier;
	}
	clients[1].socket->send(second);
}
void Net_TCP::C_Ack2IC(sf::Packet packet)
{
	int size;
	packet >> size;
	for (int i = 0; i < size; i++)
	{
		std::string ip;
		UINT16 port;
		std::string name;
		Net_UserState state;
		UINT8 id;
		if (packet >> ip >> port >> name >> state >> id)
		{
			for (int j = 1; j < clients.size(); j++)
			{
				if (clients[j].identifier == id)
				{
					clients[j].ipAddress = ip;
					clients[j].username = name;
					clients[j].state = state;

				}
			}
		}
	}
}
#pragma endregion
///Host: A client has left, tell all other clients to remove a client with the name ''
///Client: A client with name has left, erase the first client with that name.
void Net_TCP::RemoveClient(sf::Packet packet) {
	
	if (hosting == CLIENT)
	{
		std::string name;
		if (packet >> name)
		{
			//Erase the first client with name
			for (int i = 2; i < clients.size(); i++)
			{
				if (clients[i].username == name)
				{
					clients.erase(clients.begin() + i);
					return;
				}

			}
			DEBUG("Couldn't find client name in clients list \n");
			return;
		}
		else {
			DEBUG("Couldn't parse client name for removal \n");
			return;
		}
		
	}



	std::string ip;
	UINT16 port;
	if (packet >> ip >> port)
	{
		int savedIndex = 3;

		//Grabs the index of which client has left
		for (int i = 1; i < clients.size(); i++)
		{
			if ((ip == clients[i].ipAddress || clients[i].ipAddress == "127.0.0.1") && port == clients[i].port)
			{
				savedIndex = i;
			}
		}

		KickClient(savedIndex);

	}

}
void Net_TCP::Disconnect() {
	//Disconnect and destroy all info except for self
	socket->disconnect();
	selector.clear();
	while (clients.size() > 1)
	{
		clients.pop_back();
	}

	//Force leave is for if the lobby got closed.
	forceLeave = true;
}

void Net_TCP::KickClient(int clientNum) {
	//Re-organises the presence counter
	for (int i = clientNum; i < 3; i++)
	{
		presenceCounter[i - 1] = presenceCounter[i];
	}
	presenceCounter[2] = 0;


	//Grab Client username
	sf::Packet clientLeftPack;
	clientLeftPack << COM_LEAVING << clients[clientNum].username;


	//Disconnect and remove client
	selector.remove(*clients[clientNum].socket);
	clients[clientNum].socket->disconnect();
	clients.erase(clients.begin() + clientNum);

	//Tell all other clients that, a client with username has left
	for (int i = 1; i < clients.size(); i++)
	{
		sockStatus = clients[i].socket->send(clientLeftPack);
	}
}
#pragma endregion

void Net_TCP::StartGame() {
	sf::Packet startPack;
	int seed = 5;
	eventIndicesInformation.push(seed);
	startPack << COM_START << seed;

	//Tell all other clients that, a client with username has left
	for (int i = 1; i < clients.size(); i++)
	{
		clients[i].socket->send(startPack);
	}
	InitGame();
}

#pragma region PRESENCE_CHECKING

static float waitTime = 5.0f;
static const float waitInterval = 5.0f;

void Net_TCP::A_PresenceCheck(float dt) {
	waitTime -= dt;
	if (waitTime >= 0)
		return;

	
	//Clients need to check if host has not been sending presence check. Leave if host doesn't respond.
	if (hosting == CLIENT) {

		if (waitTime <= -10)//Host disconnection time is 5 - this aka 15 seconds to detect host disconnection
		{
			std::cout << "Host has disconnected" << std::endl;
			Disconnect();
		}
		return;
	}

	DEBUG("Sending Clients Presence Check. ");
	//Host resets interval and requests a presence check from all clients.
	waitTime = waitInterval;
	for (int i = 1; i < clients.size(); i++)
	{
		if (presenceCounter[i - 1] > timeoutTime) {
			std::cout << "Client: " << clients[i].username << " is no longer responding" << std::endl;
			KickClient(i);
			continue;
		}


		sf::Packet checkPack;
		checkPack << COM_PRESENCE;
		sockStatus = clients[i].socket->send(checkPack);
		presenceCounter[i - 1] += 1;
	}
}
void Net_TCP::C_SendPresenceConfirm() {
	if (hosting == HOST)
		return;

	DEBUG("Sending Presence Confirmation \n");
	sf::Packet presentPack;
	presentPack << COM_PRESENCECONFIRM << ip.toString() << clients[1].socket->getLocalPort();//Local port is mainly for confirmation for programs running locally. Uneeded for online play
	clients[1].socket->send(presentPack);

	waitTime = waitInterval;
}
void Net_TCP::S_CheckPresenceConfirm(sf::Packet packet) {
	if (hosting == CLIENT)
		return;
	DEBUG("Checking Presence Confirmation. \n");

	//Resets the counters for the client that sent a presence check.
	std::string ip;
	UINT16 port;
	if (packet >> ip >> port)
	{
		for (int i = 1; i < clients.size(); i++)
		{
			if ((ip == clients[i].ipAddress || clients[i].ipAddress == "127.0.0.1") && port == clients[i].port)//Check if it's a real IP. If we have stored the user as a local one, just check it's local
			{
				presenceCounter[i - 1] = 0;
				return;
			}


		}

	}
	else
		DEBUG("Unable to parse IP/Port for presence \n");
	DEBUG("Client presence unable to confirm, no clients changed \n");
}
#pragma endregion

void Net_TCP::InitGame() {
	sf::Packet initPack;
	DEBUG("Initialising Game Clients\n");
	if (hosting == HOST)
	{
		//This is to tell all clients, which user is starting where. AKA host will always start at position 0, client 1 will start at position 1.
		for (int i = 0; i < clients.size(); i++)
		{
			initPack << clients[i].username;
			clients[i].identifier = i;
			//clients[i].state.position = sf::Vector2f(1000 + (100 * i), 1000+ (100 * i));
			clients[i].state.position = sf::Vector2f(1000, 1000);
			//HOST 0
			//CLIENT 1
		}
		for (int i = 1; i < clients.size(); i++)
		{
			clients[i].socket->send(initPack);
		}
		//S_Designate2IC();
		DEBUG("Finished Initialisation\n");

		return;
	}

	//Assigns identifier and position to each client
	//TODO: There might be a possible issue we run into in the future where people having the same names can cause... issues.
	//A fix for that would probably be having the identifier set as soon as someone joins the game
	clients[1].socket->setBlocking(true);
	clients[1].socket->receive(initPack);

	for (int i = 0; i < clients.size(); i++)
	{
		std::string name;
		if (initPack >> name)
		{
			for (int j = 0; j < clients.size(); j++)
			{
				if (name == clients[j].username)
				{
					
					clients[j].identifier = i;
					//clients[j].state.position = sf::Vector2f(1000 + (100*i), 1000 + (100 * i));
					clients[i].state.position = sf::Vector2f(1000, 1000);
				}
			}
		}
	}
	clients[1].socket->setBlocking(false);
	DEBUG("Finished Initialisation\n");

}


void Net_TCP::UpdateState(Net_UserState state, int identifier) {
	//This exists in case we are not updating self
	//Packet structure :: Update :: Identifier :: Net_UserState
	sf::Packet updatePack;
	updatePack << COM_GAMEUPDATE << identifier << state;
	//If we're host, send to everyone
	//If we're client, send to host -> who will then send to everyone
	if (hosting == HOST)
	{
		for (int i = 1; i < clients.size(); i++)
		{
			if (identifier == clients[i].identifier)
				continue;
			clients[i].socket->send(updatePack);
		}
	}
	else {
		clients[1].socket->send(updatePack);
	}

}
void Net_TCP::UpdateState(Net_UserState state) {

	//We send our state update and apply it to ourselves. See overloaded function to see the other side
	sf::Packet updatePack;
	updatePack << COM_GAMEUPDATE << clients[0].identifier << state;
	if (hosting == HOST)
	{
		for (int i = 1; i < clients.size(); i++)
		{
			clients[i].socket->send(updatePack);
		}
	}
	else {
		clients[1].socket->send(updatePack);
	}

}

void Net_TCP::SendMessage(sf::Packet packet) {
	//Send a custom packet
	//Will still need to be parsed properly on the other end
	if (hosting == HOST)
	{
		for (int i = 1; i < clients.size(); i++)
		{
			clients[i].socket->send(packet);
		}
	}
	else {
		clients[1].socket->send(packet);
	}
}

void Net_TCP::SendMessage(sf::Packet packet, int identifier)
{

	if (hosting == HOST)
	{
		for (int i = 1; i < clients.size(); i++)
		{
			if (identifier == clients[i].identifier)
				continue;
			clients[i].socket->send(packet);
		}
	}
	else {
		//Clients shouldn't be here
	}
}


void Net_TCP::InterpretUpdate(sf::Packet packet) {
	//This function will update game states
	int identifier;
	packet >> identifier;

	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].identifier == identifier)
		{

			Net_UserState newState;
			packet >> newState;

			clients[i].state = newState;
			if (hosting == HOST)
			{
				UpdateState(newState, identifier);
			}
		}
	}
}