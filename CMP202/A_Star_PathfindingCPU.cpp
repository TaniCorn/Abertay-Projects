#include <thread>

#include <future>
#include "A_Star_PathfindingCPU.h"
#include <chrono>

std::vector<ExportData> ExportBenchmark::data;
std::vector<ExportData> ExportBenchmark::data2;
std::vector<ExportData> ExportBenchmark::data4;
std::vector<ExportData> ExportBenchmark::data8;
std::vector<ExportData> ExportBenchmark::data16;
std::vector<ExportData>* ExportBenchmark::current = &data;

#pragma region BASE_ASTAR
bool Base_A_Star_PathfindingCPU::SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos)
{
	//Run Async functions to find node from positions
	std::future<Node*> startNode_fut = std::async(std::launch::async, &Base_A_Star_PathfindingCPU::GetNodeFromPosition,this, startPos);
	std::future<Node*> endNode_fut = std::async(std::launch::async, &Base_A_Star_PathfindingCPU::GetNodeFromPosition,this, endPos);

	//Set up root node
	Node* startNode = startNode_fut.get();
	if (startNode == nullptr)
		return false;
	startNode->SetGCost(0); startNode->SetParent(nullptr);
	startNode->SetHCost(floorf((endPos.x - startPos.x) + (endPos.y - startPos.y))); startNode->SetFCost();
	root = startNode;
	
	//Set up target node
	target = endNode_fut.get();
	if (target == nullptr)
		return false;

	//Set up root room
	for (Room* var : rooms)
	{
		if (IsNodeInRoom(*var, Vector2<int>(startNode->xPosition, startNode->yPosition)))
		{
			startRoom = var;
			return true;
		}
	}


	return false;
}
/// <summary>
/// Searches all rooms to find the corresponding node with the same position as given
/// </summary>
Node* Base_A_Star_PathfindingCPU::GetNodeFromPosition(Vector2<int> position)
{
	//Get node from rooms based on position
	for (auto rm : rooms)
	{
		if (IsNodeInRoom(*rm, position))
		{
			return FindNodeInRoom(position, rm);
		}
	}
}
/// <summary>
/// Finds and returns the node in the room given, assuming the coordinates match. Will shift as necessary for any position not comforming to the node size
/// </summary>
Node* Base_A_Star_PathfindingCPU::FindNodeInRoom(Vector2<int> pos, Room* rm)
{
	//Trying to get the reference of the room position back to 0 to equalise everything
	Vector2<int> balance = Vector2<int>(rm->GetLowestXCoord(), rm->GetLowestYCoord());
	Vector2<int> tempPos = pos;
	tempPos -= balance;

	//We are now working in an assumed 1x1 grid. Get array from grid position
	int x, y;
	x = 0;
	y = 0;
	x = tempPos.x; y = tempPos.y;
	
	return &rm->nodes[x][y];

}
///////Acknowledgment: The IsNodeInRoom functions are based on the CMP105 project - collision detection
//Collision detection code for finding if position is within certain bounds
bool Base_A_Star_PathfindingCPU::IsNodeInRoom(const RoomStruct& nm, const Vector2<int> position)
{
	if (nm.GetHighestXCoord() < position.x) { return false; }
	if (nm.GetLowestXCoord() > position.x) { return false; }
	if (nm.GetHighestYCoord() < position.y) { return false; }
	if (nm.GetLowestYCoord() > position.y) { return false; }

	return true;
}


void Base_A_Star_PathfindingCPU::EditNode(Vector2<int> nodePos, NodeType changeTo)
{
	//Get the node
	Node* changedNode = nullptr;
	changedNode = GetNodeFromPosition(nodePos);
	if (changedNode == nullptr)
		return;

	if (changedNode->nodeType == Routes)
		return;
	//Change the node
	changedNode->nodeType = changeTo;
}
void Base_A_Star_PathfindingCPU::PrintRoute()
{
	//Changes most recursive nodes parents to 'Path' from target node
	Node* cn; cn = target;
	if (cn == nullptr)
		return;
	while (cn->GetParent() != nullptr)
	{
		if (cn->nodeType != Routes)
		{
			cn->nodeType = Path;
		}
		cn = cn->GetParent();
	}

	//Changes root node
	if (cn->nodeType != Routes)
	{
		cn->nodeType = Free;
	}

}
void Base_A_Star_PathfindingCPU::ResetRoute()
{
	//Changes most recursive nodes parents to 'Free' from target node and sets parent to nullptr afterwards
	Node* cn; cn = target;
	if (cn == nullptr)
		return;
	while (cn->GetParent() != nullptr)
	{
		if (cn->nodeType != Routes)
		{
			cn->nodeType = Free;
		}
		Node* oldCN = cn;
		cn = cn->GetParent();
		oldCN->SetParent(nullptr);
	}

	//Resets root node
	if (cn->nodeType != Routes)
	{
		cn->nodeType = Free;
	}

	//Reset Rooms
	for (auto rm : rooms)
	{
		if (rm->GetParentRoom() != nullptr)
		{
			rm->SetParentRoom(nullptr);
		}
	}
}
#pragma endregion


#pragma region SEGMENTED

/// <summary>
/// Specific to segmented variant
/// </summary>
bool A_Star_Pathfinding_Defined_SegmentedCPU::AStarAlgorithm()
{
	//This function will find the applicable Route Nodes it needs to travel to and then initiate a sequence of pathfinding agents to go from Route Node to Route Node
	//This function is not parallisabel, many child functions are

	std::queue<Node*> tempTargets;//temporary target nodes that join rooms together towards main target node - these derive from the 'route' nodes that connect rooms

	//Search for a room path and push the 'Route' nodes into tempTargets
	if (!IsNodeInRoom(*startRoom, Vector2<int>(target->xPosition, target->yPosition))) {
		tempTargets = FindRouteNodePaths(BruteForcePathfindMaps());//temporary target nodes that join rooms together towards main target node
	}
	else {
		tempTargets.push(root);//Required for same room pathfinding
	}
	tempTargets.push(target);

	return MultiThreadedPathfinding(&tempTargets);
}
std::queue<Node*> A_Star_Pathfinding_Defined_SegmentedCPU::FindRouteNodePaths(std::stack<RoomStruct*> mapRoute)
{
	//Using the map route, find the corrseponding 'route' nodes that would connect the rooms together
	//This function is not parallisable, but a child function is

	std::queue<Node*> temporaryTargets;//'Route'.teleportTo nodes to travel to, on the way towards main end node
	temporaryTargets.push(root);

	while (!mapRoute.empty())
	{
		Node* rn = FindRouteNode(mapRoute);
		if (rn == nullptr)
		{
			//We have not found a path or mapRoute is now empty
			break;
		}
		temporaryTargets.push(rn);
	}
	return temporaryTargets;
}
std::stack<RoomStruct*> A_Star_Pathfinding_Defined_SegmentedCPU::BruteForcePathfindMaps()
{
	std::stack<RoomStruct*> mapRoute;//Rooms we need to go through
	RoomStruct* currentRoomToSearch = startRoom;//Our current room that we're in

	std::set<RoomStruct*> open; open.insert(currentRoomToSearch);
	std::set<RoomStruct*> closed;
	//Brute force searching of all rooms
	while (open.size() != 0)
	{


		currentRoomToSearch = *open.begin();
		open.erase(open.begin());
		closed.insert(currentRoomToSearch);

#pragma region parallisable_start
		//Search all neighbouring rooms
		for (auto neighbour : currentRoomToSearch->GetNeighbouringRooms())
		{
			if (closed.find(neighbour) != closed.end())
			{
				continue;
			}
			neighbour->SetParentRoom(currentRoomToSearch);

			//If room is our target, start pushing path into mapRoute//otherwise add it to the open set
			if (IsNodeInRoom(*neighbour, Vector2<int>(target->xPosition, target->yPosition)))
			{
				//Push room path into mapRoute and return
				RoomStruct* route = neighbour;
				while (route->GetParentRoom() != nullptr)
				{
					mapRoute.push(route);
					route = route->GetParentRoom();
				}
				mapRoute.push(route);
				return mapRoute;
			};

			open.insert(neighbour);
		}
#pragma endregion
	}

	return mapRoute;//Returns an empty std::stack
}
Node* A_Star_Pathfinding_Defined_SegmentedCPU::FindRouteNode(std::stack<RoomStruct*>& mapRoute)
{
	//Searches through all the Rooms and collects the Route Nodes that connect them to eachother
	//This function is partially parallisable

	RoomStruct* map = mapRoute.top();
	mapRoute.pop();
	//Same Room search
	if (mapRoute.size() == 0)
	{
		return nullptr;
	}

#pragma region parallisable_start
	//Search all the Route Nodes of the Room to check if they teleport to the next Room
	for (auto routeNode : map->GetRouteNodes())
	{
		Node* nodeInNextRoom = routeNode->nodeToTeleportTo;
		if (nodeInNextRoom != nullptr)
		{
			if (IsNodeInRoom(*mapRoute.top(), Vector2<int>(nodeInNextRoom->xPosition, nodeInNextRoom->yPosition)))//If the routeNode's teleporting node is in the next room, return the routeNode
			{
				return routeNode;
			}
		}

	}
#pragma endregion

	return nullptr;
}

bool A_Star_Pathfinding_Defined_SegmentedCPU::MultiThreadedPathfinding(std::queue<Node*>* targetNodes)
{
	//This function will go through all target nodes and send a pathfinding agent from one to the next.

	bool successful = true;
	std::vector<std::thread> threads;
	std::mutex lock;
	bool once = false;

	for (int numberOfThreads = 0; numberOfThreads < threadsToUse; numberOfThreads++)
	{
		threads.push_back(std::thread([&]() {
			//Pathfind to each target node until we run out - (by that point we should be in the same room as the target node)
			while (!targetNodes->empty())
			{
				lock.lock();

					if (targetNodes->empty())
					{
						lock.unlock();
						return;
					}

				//First get root, then the nodeToTeleportTo from all other route nodes
				Node* tempStartNode = targetNodes->front();
					if (tempStartNode->nodeType == Routes)
					{
						TeleportNode* tpNode = static_cast<TeleportNode*>(tempStartNode);
						tpNode->nodeToTeleportTo->SetParent(tpNode);//TODO: I feel like I can get rid of this by simply making sure all 
						//teleported nodes set their teleporting node as parents
						tempStartNode = tpNode->nodeToTeleportTo;
					}

				targetNodes->pop();
					if (targetNodes->empty())//This check is to prevent an error when reaching the end of the queue
					{
						lock.unlock();
						return;
					}
				Node* tempTargetNode = targetNodes->front();
				lock.unlock();

				if (!DefaultAStar(tempStartNode, tempTargetNode))
				{
					successful = false;
				}
		
			}
			}));
	}

	std::for_each(threads.begin(), threads.end(), [](std::thread& t)
		{
			t.join();
		});
	if (successful)
	{
		return true;
	}
	return false;
}



/// <summary>
/// Designed to be as modular in use as possible
/// </summary>
bool A_Star_Pathfinding_Defined_SegmentedCPU::DefaultAStar(Node* startNode, Node* endNode)
{
	//This function is the default implementation of A*.
	//This function is probably parallisable
	//I'm thinking we can set up a farming on the current.open.begin HOWEVER CheckNeighbours would be blocked. This is incase two threads are saying one node is better than the previous and they both try to add it. So there would not be performance boost really
	
	//I tried what I said below, by using a threadpool task based system. Unfortunately there are many complications with std::set and ReverseComparator. I believe these data structures are not thread safe.
	// Even when using mutexes to avoid data races, it would still fail. I will have to conclude that it is not possible to parallise it that way
	// //Second Update: I decided to give it another shot - i've come to two conclusions. 1 is that semaphores would be required to avoid a deadlock. 2 I believe it's actually slower. Looking at the instrumentation values, using the threaded version takes more time on average
	////Failed:We could also change the checkneighbours function to check a specific neighbour number. This way we can have threads running on each neighbour. We should however not close these threads and they should be able to modify the existing data.
	std::set<Node*, ReverseComparator> open;
	std::set<Node*> closed;
	open.insert(startNode);

	int iterations = 0;
	while (open.size() != 0 && iterations <= 1000000)
	{

		//Find lowest fCost Open Node
		Node* current = *open.begin();

		//If we found end, stop pathfinding
		if (Node::GetDistance(*current, *endNode) < 1)
		{
			return true;
		}
		std::set<Node*>::iterator it = open.begin();//For some reason, it won't erase some points, so we need to point to the first one to erase instead of erasing the specific node

		//Restructure the node collections
		open.erase(it);
		closed.insert(current);

		//Neighbours
		CheckNeighbours(current, endNode, &open, &closed);

		iterations++;
	}
	return false;
}


void A_Star_Pathfinding_Defined_SegmentedCPU::CheckNeighbours(Node* current, Node* targetNode, std::set<Node*, ReverseComparator>* open, std::set<Node*>* closed)
{
	//SUMMARY: Neighbours does not gain any advantage by being multithreaded implicitly(In the function) - The cost of creating and joining threads may be too high to see a significant difference
#pragma region ORIGINAL
	//For all neighbours : categorise them
	for (auto neighbour : current->neighbours)
	{
		//Parent node
		if (neighbour == nullptr || neighbour->nodeType == Obstacle || closed->find(neighbour) != closed->end())
		{
			continue;
		}

		//Target Node
		if (neighbour->xPosition == targetNode->xPosition && neighbour->yPosition == targetNode->yPosition)
		{
			targetNode->SetParent(current);
			targetNode->SetFCost(0);
		}

		int newGCost = current->GetGCost() + Node::GetDistance(*neighbour, *current);

		bool inToSearch = open->find(neighbour) != open->end();
		//If neighbour is in either open, and the new path is better, recalculate
		if (inToSearch && neighbour->GetGCost() > newGCost)
		{
			int newHCost = Node::GetDistance(*neighbour, *targetNode);
			int newFCost = newGCost + newHCost;
			open->erase(neighbour);//This is needed otherwise it will not re-order the element when new values are put in
			neighbour->SetGCost(newGCost);
			neighbour->SetHCost(newHCost);
			neighbour->SetFCost(newFCost);
			neighbour->SetParent(current);
			open->insert(neighbour);
		}
		//If neighbour is not in open, set
		if (!inToSearch)
		{
			int newHCost = Node::GetDistance(*neighbour, *targetNode);
			int newFCost = newGCost + newHCost;
			neighbour->SetGCost(newGCost);
			neighbour->SetHCost(newHCost);
			neighbour->SetFCost(newFCost);
			neighbour->SetParent(current);
			open->insert(neighbour);
		}

	}
#pragma endregion
