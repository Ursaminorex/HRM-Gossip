#include "Graph.h"
#include <algorithm>


static int currentHop = 0;
Graph::Graph(int _nV, int _nE, int _maxOd)
{
	m_nV = _nV;
	m_nE = _nE;
	m_maxOd = _maxOd;
	m_maxHop = 1000;
	m_w = 0.8f;
	m_adjList = new list<int>[m_nV];
	m_nodes = new Node[m_nV];
	m_adjMatrix = new int* [m_nV];
	for (int i = 0; i < m_nV; i++)
	{
		m_adjMatrix[i] = new int [m_nV] {0};	// 初始化邻接矩阵
	}
}

Graph::~Graph()
{
	if (m_adjList)
	{
		delete[] m_adjList;
	}
	if (m_nodes)
	{
		delete[] m_nodes;
	}
	for (int i = 0; i < m_nV; ++i)
	{
		if (m_adjMatrix[i])
		{
			delete[] m_adjMatrix[i];
		}
	}
	if (m_adjMatrix)
	{
		delete[] m_adjMatrix;
	}
}

void Graph::SetWeight(float _w)
{
	this->m_w = _w;
}

void Graph::SetMaxHop(int _maxhop)
{
	this->m_maxHop = _maxhop;
}

int Graph::GetVnum()
{
	return m_nV;
}



void Graph::PrintLoadDistribution()
{

}

void Graph::RandCreate()
{
	int ra, rb;  //两个随机数，用于记录各节点的度 
	int count = 0;    //只有m条边，count来记录当前生成的边数  

	while (count < m_nE)
	{
		ra = rand() % m_nV;//生成数组随机位置 
		rb = rand() % m_nV;
		while (ra == rb)
			rb = rand() % m_nV;//这两个随机数必须是不相等 

		if (!m_adjMatrix[ra][rb] && m_nodes[ra].od < m_maxOd)
		{
			m_adjMatrix[ra][rb] = 1;
			AddEdge(ra, rb);
			//m_adjMatrix[ra][rb] = m_adjMatrix[rb][ra] = rand() % 9 + 1;//对称生成邻接矩阵,随机生成权值， 
			count++;
			m_nodes[ra].od++;
			m_nodes[rb].id++;
		}
	}
	this->calculateTau();	//计算节点冷热度
}

void Graph::AddEdge(int v, int w)
{
	m_adjList[v].push_back(w);
}

void Graph::RemoveEdge(int v, int w)
{
	m_adjList[v].remove(w);
}

void Graph::HRM(int activeNode, int passiveNode)
{
	int recommendNodeAddr, replaceNodeAddr;
	//查找主动节点最冷邻居
	recommendNodeAddr = getColdestNeighborAddr(activeNode);
	//如果最冷节点是被动节点，则选择倒数第二冷节点
	if (passiveNode == recommendNodeAddr)
	{
		if (m_nodes[activeNode].od == 1)
		{	// 如果主动节点邻居数为1且该邻居恰好是被动节点则跳过
			printf("%d's od == %d and the recommendNode is same as %d pass!\n", activeNode, m_nodes[activeNode].od, passiveNode);
			return;
		}
		recommendNodeAddr = getSecondLastColdNeighborAddr(activeNode, recommendNodeAddr);
	}

	printf("%d:[%d]->%d\n", activeNode, recommendNodeAddr, passiveNode);

	if (m_adjMatrix[passiveNode][recommendNodeAddr] == 1)
	{	// 已经是邻居
		printf("%d has %d\n", passiveNode, recommendNodeAddr);
		return;
	}

	//this->PrintView();

	if (m_nodes[passiveNode].od < m_maxOd)
	{
		this->AddEdge(passiveNode, recommendNodeAddr);
		++m_nE;	// 边数+1
		m_adjMatrix[passiveNode][recommendNodeAddr] = 1;
		m_nodes[passiveNode].od++;
		m_nodes[recommendNodeAddr].id++;
		m_nodes[recommendNodeAddr].tau++;
		printf("%d:[%d]->%d\n", activeNode, recommendNodeAddr, passiveNode);
		printf("add %d:[%d]\n", passiveNode, recommendNodeAddr);
	}
	else 
	{	// 若邻居已满，需要先移除最热邻居
		// 查找被动节点最热邻居
		replaceNodeAddr = getHottestNeighborAddr(passiveNode);
		// 移除被动节点最热邻居
		this->RemoveEdge(passiveNode, replaceNodeAddr);
		m_adjMatrix[passiveNode][replaceNodeAddr] = 0;
		m_nodes[replaceNodeAddr].id--;
		m_nodes[replaceNodeAddr].tau--;
		this->AddEdge(passiveNode, recommendNodeAddr);
		m_adjMatrix[passiveNode][recommendNodeAddr] = 1;
		m_nodes[recommendNodeAddr].id++;
		m_nodes[recommendNodeAddr].tau++;
		printf("%d:[%d]->%d\n", activeNode, recommendNodeAddr, passiveNode);
		printf("%d:[%d]->%d:[%d]\n", passiveNode, replaceNodeAddr, passiveNode, recommendNodeAddr);
	}
	printAdjList(passiveNode);
}

Graph* Graph::GetTranspose()		// 有向图反向
{
	Graph* g = new Graph(m_nV, m_nE, m_maxOd);
	for (int i = 0; i < m_nV; i++)
	{
		for (list<int>::iterator it = m_adjList[i].begin(); it != m_adjList[i].end(); it++)
		{
			g->AddEdge(*it, i);		// 反向添加 
		}
	}
	return g;
}
void Graph::DFSUtil(int u, bool visited[])  //DFS
{
	visited[u] = true;
	list<int>::iterator it;
	for (it = m_adjList[u].begin(); it != m_adjList[u].end(); it++)
	{
		if (!visited[*it])
		{
			DFSUtil(*it, visited);
		}
	}
}
bool Graph::IsSc()
{	// kosaraju算法
	bool* visited = new bool[m_nV];
	for (int i = 0; i < m_nV; i++)
	{
		visited[i] = false;
	}
	DFSUtil(0, visited); 				//从0开始DFS 
	//BFSUtil(0, visited); 				//从0开始BFS 
	for (int i = 0; i < m_nV; i++)
	{
		if (visited[i] == false) 		//不连通 
		{
			return false;
		}
	}
	Graph* gr = GetTranspose(); 		//得到反向的有向图 
	for (int i = 0; i < m_nV; i++)
	{
		visited[i] = false;
	}
	gr->DFSUtil(0, visited); 			  //从0开始对反向的有向进行DFS 
	//gr.BFSUtil(0, visited); 			  //从0开始对反向的有向进行BFS 
	for (int i = 0; i < m_nV; i++)
	{
		if (visited[i] == false) 		//不连通 
		{
			return false;
		}
	}
	return true;
}

int Graph::StartGossip(int mode)
{
	bool* received = new bool[m_nV]{false};
	int firstPeer = rand() % m_nV;
	int hop;
	currentHop = 0;
	if (mode)
	{
		hop = this->gossipWithHRM(firstPeer, received, m_maxHop);
	}
	else
	{
		hop = this->gossip(firstPeer, received, m_maxHop);
	}

	this->saveMToQueue();	// 完成一次Gossip后各个节点保存该轮收到总消息数到缓存队列中
	this->calculateTau();	//计算节点冷热度

	for (int i = 0; i < m_nV; i++)
	{
		if (m_nodes[i].id < 1 || m_nodes[i].avgLoad < 1)	 // 孤立节点
		{
			for (list<int>::iterator it = m_adjList[i].begin(); it != m_adjList[i].end(); it++)
			{
				int recommendNodeAddr = i, passiveNode = *it;
				int replaceNodeAddr;
				if (m_nodes[passiveNode].od < m_maxOd)
				{
					this->AddEdge(passiveNode, recommendNodeAddr);
					++m_nE;	// 边数+1
					m_adjMatrix[passiveNode][recommendNodeAddr] = 1;
					m_nodes[passiveNode].od++;
					m_nodes[recommendNodeAddr].id++;
					m_nodes[recommendNodeAddr].tau++;
					printf("add %d:[%d]\n", passiveNode, recommendNodeAddr);
				}
				else
				{	// 若邻居已满，需要先移除最热邻居
					// 查找被动节点最热邻居
					replaceNodeAddr = getHottestNeighborAddr(passiveNode);
					// 移除被动节点最热邻居
					this->RemoveEdge(passiveNode, replaceNodeAddr);
					m_adjMatrix[passiveNode][replaceNodeAddr] = 0;
					m_nodes[replaceNodeAddr].id--;
					m_nodes[replaceNodeAddr].tau--;
					this->AddEdge(passiveNode, recommendNodeAddr);
					m_adjMatrix[passiveNode][recommendNodeAddr] = 1;
					m_nodes[recommendNodeAddr].id++;
					m_nodes[recommendNodeAddr].tau++;
					printf("%d:[%d]->%d:[%d]\n", passiveNode, replaceNodeAddr, passiveNode, recommendNodeAddr);
				}
				if (received)
				{
					delete[] received;
				}
				received = new bool[m_nV] {false};
				this->gossipWithHRM(passiveNode, received, 3);
			}
		}
	}

	return hop;
}

int Graph::gossip(int activePeer, bool received[], int maxHop)
{
	received[activePeer] = true;	//表示受到过消息
	++m_nodes[activePeer].m;	//表示收到一条消息

	if (++currentHop >= maxHop)
	{
		return 1;
	}
	for (int i = 0; i < m_nV; i++)
	{
		if (!received[i])
		{
			if (m_adjList[activePeer].size() <= 0)
			{
				return 1;
			}
			int randNei = rand() % m_adjList[activePeer].size();
			list<int>::iterator it = m_adjList[activePeer].begin();
			int passivePeer = activePeer;
			for (int j = 0; j <= randNei; j++)
			{
				passivePeer = *it;
				it++;
			}
			if (activePeer == passivePeer)
			{
				int temp = randNei;
				randNei = rand() % m_adjList[activePeer].size();
				while (temp == randNei)
				{
					randNei = rand() % m_adjList[activePeer].size();	//这两个随机数必须是不相等 
				}
				for (int j = 0; j <= randNei; j++)
				{
					passivePeer = *it;
					it++;
				}
			}
			//if (m_nodes[activePeer].mq.size() == mq_len)
			//{
			//	HRM(activePeer, passivePeer);
			//}
			return 1 + gossip(passivePeer, received, maxHop);
		}
	}
	return 1;
}

int Graph::gossipWithHRM(int activePeer, bool received[], int maxHop)
{
	received[activePeer] = true;	//表示受到过消息
	++m_nodes[activePeer].m;	//表示收到一条消息

	if (++currentHop >= maxHop)
	{
		return 1;
	}
	for (int i = 0; i < m_nV; i++)
	{
		if (!received[i])
		{
			if (m_adjList[activePeer].size() <= 0)
			{
				return 1;
			}
			int randNei = rand() % m_adjList[activePeer].size();
			list<int>::iterator it = m_adjList[activePeer].begin();
			int passivePeer = activePeer;
			for (int j = 0; j <= randNei; j++)
			{
				passivePeer = *it;
				it++;
			}
			if (activePeer == passivePeer)
			{
				int temp = randNei;
				randNei = rand() % m_adjList[activePeer].size();
				while (temp == randNei)
				{
					randNei = rand() % m_adjList[activePeer].size();	//这两个随机数必须是不相等 
				}
				for (int j = 0; j <= randNei; j++)
				{
					passivePeer = *it;
					it++;
				}
			}
			HRM(activePeer, passivePeer);
			return 1 + gossipWithHRM(passivePeer, received, maxHop);
		}
	}
	return 1;
}

void Graph::JoinPeers(int count)
{
	// 创建临时变量保存类成员，重新申请动态内存
	list<int>*new_adjList = new list<int>[m_nV];
	p_node new_nodes = new Node[m_nV];
	int** new_adjMatrix = new int* [m_nV];
	for (int i = 0; i < m_nV; i++)
	{
		new_adjMatrix[i] = new int [m_nV];
	}

	// 拷贝节点数组、邻接矩阵、邻接表
	for (int i = 0; i < m_nV; i++)
	{
		new_nodes[i] = m_nodes[i];
		for (int j = 0; j < m_nV; j++)
		{
			new_adjMatrix[i][j] = m_adjMatrix[i][j];
		}
		for (list<int>::iterator it = m_adjList[i].begin(); it != m_adjList[i].end(); it++)
		{
			new_adjList[i].push_back(*it);
		}
	}

	// 重新申请节点数组、邻接矩阵和邻接表的空间
	if (m_adjList)
	{
		delete[] m_adjList;
	}
	if (m_nodes)
	{
		delete[] m_nodes;
	}
	for (int i = 0; i < m_nV; ++i)
	{
		if (m_adjMatrix[i])
		{
			delete[] m_adjMatrix[i];
		}
	}
	if (m_adjMatrix)
	{
		delete[] m_adjMatrix;
	}

	m_nV += count;
	m_adjList = new list<int>[m_nV];
	m_nodes = new Node[m_nV];
	m_adjMatrix = new int* [m_nV];
	for (int i = 0; i < m_nV; i++)
	{
		m_adjMatrix[i] = new int[m_nV];
	}

	// 拷贝节点数组、邻接矩阵、邻接表
	for (int i = 0; i < m_nV - count; i++)
	{
		m_nodes[i] = new_nodes[i];
		for (int j = 0; j < m_nV; j++)
		{
			m_adjMatrix[i][j] = new_adjMatrix[i][j];
		}
		for (list<int>::iterator it = new_adjList[i].begin(); it != new_adjList[i].end(); it++)
		{
			m_adjList[i].push_back(*it);
		}
	}

	// 释放临时变量
	if (new_adjList)
	{
		delete[] new_adjList;
	}
	if (new_nodes)
	{
		delete[] new_nodes;
	}
	for (int i = 0; i < m_nV - count; ++i)
	{
		if (new_adjMatrix[i])
		{
			delete[] new_adjMatrix[i];
		}
	}
	if (new_adjMatrix)
	{
		delete[] new_adjMatrix;
	}

	//随机添加最大出度一半值的边，并依次发起gossipWithHRM算法
	for (int i = m_nV - count; i < m_nV; i++)
	{
		vector<int> temp;
		for (int j = 0; j < m_nV - count; j++)
		{
			temp.push_back(j);
		}

		random_shuffle(temp.begin(), temp.end());

		for (int k = 0; k < m_maxOd / 2; k++)
		{
			this->AddEdge(i, temp[k]);
			m_adjMatrix[i][temp[k]] = 1;
			m_nodes[i].od++;
			m_nodes[temp[k]].id++;
			m_nE++;

			int recommendNodeAddr = i, passiveNode = temp[k];
			int replaceNodeAddr;
			if (m_nodes[passiveNode].od < m_maxOd)
			{
				this->AddEdge(passiveNode, recommendNodeAddr);
				++m_nE;	// 边数+1
				m_adjMatrix[passiveNode][recommendNodeAddr] = 1;
				m_nodes[passiveNode].od++;
				m_nodes[recommendNodeAddr].id++;
				m_nodes[recommendNodeAddr].tau++;
				printf("add %d:[%d]\n", passiveNode, recommendNodeAddr);
			}
			else
			{	// 若邻居已满，需要先移除最热邻居
				// 查找被动节点最热邻居
				replaceNodeAddr = getHottestNeighborAddr(passiveNode);
				// 移除被动节点最热邻居
				this->RemoveEdge(passiveNode, replaceNodeAddr);
				m_adjMatrix[passiveNode][replaceNodeAddr] = 0;
				m_nodes[replaceNodeAddr].id--;
				m_nodes[replaceNodeAddr].tau--;
				this->AddEdge(passiveNode, recommendNodeAddr);
				m_adjMatrix[passiveNode][recommendNodeAddr] = 1;
				m_nodes[recommendNodeAddr].id++;
				m_nodes[recommendNodeAddr].tau++;
				printf("%d:[%d]->%d:[%d]\n", passiveNode, replaceNodeAddr, passiveNode, recommendNodeAddr);
			}
		}
		this->printAdjList(i);
	}
}

void Graph::LeavePeers(int count)
{
	// 创建临时变量保存类成员，重新申请动态内存
	list<int>* new_adjList = new list<int>[m_nV];
	p_node new_nodes = new Node[m_nV];
	int** new_adjMatrix = new int* [m_nV];
	for (int i = 0; i < m_nV; i++)
	{
		new_adjMatrix[i] = new int[m_nV];
	}

	// 拷贝节点数组、邻接矩阵、邻接表
	for (int i = 0; i < m_nV; i++)
	{
		new_nodes[i] = m_nodes[i];
		for (int j = 0; j < m_nV; j++)
		{
			new_adjMatrix[i][j] = m_adjMatrix[i][j];
		}
		for (list<int>::iterator it = m_adjList[i].begin(); it != m_adjList[i].end(); it++)
		{
			new_adjList[i].push_back(*it);
		}
	}

	// 重新申请节点数组、邻接矩阵和邻接表的空间
	if (m_adjList)
	{
		delete[] m_adjList;
	}
	if (m_nodes)
	{
		delete[] m_nodes;
	}
	for (int i = 0; i < m_nV; ++i)
	{
		if (m_adjMatrix[i])
		{
			delete[] m_adjMatrix[i];
		}
	}
	if (m_adjMatrix)
	{
		delete[] m_adjMatrix;
	}

	//删除掉线节点的所有边
	for (int i = m_nV - count; i < m_nV; i++)
	{
		m_nE -= (int)new_adjList[i].size();
		for (list<int>::iterator it = new_adjList[i].begin(); it != new_adjList[i].end(); it++)
		{
			new_nodes[*it].id--;
		}

		for (int j = 0; j < m_nV; j++)
		{
			for (list<int>::iterator it = new_adjList[j].begin(); it != new_adjList[j].end(); it++)
			{
				if (*it == i)
				{
					new_adjList[j].erase(it);
					new_nodes[j].od--;
					break;
				}
			}
		}
		printf("remove %d\n", i);
	}

	m_nV -= count;
	m_adjList = new list<int>[m_nV];
	m_nodes = new Node[m_nV];
	m_adjMatrix = new int* [m_nV];
	for (int i = 0; i < m_nV; i++)
	{
		m_adjMatrix[i] = new int[m_nV];
	}

	// 拷贝节点数组、邻接矩阵、邻接表
	for (int i = 0; i < m_nV; i++)
	{
		m_nodes[i] = new_nodes[i];
		for (int j = 0; j < m_nV; j++)
		{
			m_adjMatrix[i][j] = new_adjMatrix[i][j];
		}
		for (list<int>::iterator it = new_adjList[i].begin(); it != new_adjList[i].end(); it++)
		{
			m_adjList[i].push_back(*it);
		}
	}

	// 释放临时变量
	if (new_adjList)
	{
		delete[] new_adjList;
	}
	if (new_nodes)
	{
		delete[] new_nodes;
	}
	for (int i = 0; i < m_nV; ++i)
	{
		if (new_adjMatrix[i])
		{
			delete[] new_adjMatrix[i];
		}
	}
	if (new_adjMatrix)
	{
		delete[] new_adjMatrix;
	}
}

int Graph::getColdestNeighborAddr(int nodeAddr)
{
	list<int>::iterator it = m_adjList[nodeAddr].begin();
	int coldestNeiAddr = *it;
	it++;
	for (; it != m_adjList[nodeAddr].end(); it++)
	{
		coldestNeiAddr = getColderAddr(coldestNeiAddr, *it);
	}

	return coldestNeiAddr;
}

int Graph::getHottestNeighborAddr(int nodeAddr)
{
	list<int>::iterator it = m_adjList[nodeAddr].begin();
	int hottestNeiAddr = *it;
	it++;
	for (; it != m_adjList[nodeAddr].end(); it++)
	{
		hottestNeiAddr = getHotterAddr(hottestNeiAddr, *it);
	}

	return hottestNeiAddr;
}

int Graph::getSecondLastColdNeighborAddr(int nodeAddr, int coldestAddr)
{
	// 考虑只有一个邻居的情况
	list<int>::iterator it = m_adjList[nodeAddr].begin();
	if (*it == coldestAddr)
	{
		it++;
	}
	int secondLastColdNeiAddr = *it;
	it++;
	for (; it != m_adjList[nodeAddr].end(); it++)
	{
		if (*it == coldestAddr)
		{
			continue;
		}
		secondLastColdNeiAddr = getColderAddr(secondLastColdNeiAddr, *it);
	}

	return secondLastColdNeiAddr;
}

int Graph::getColderAddr(int n1, int n2)
{
	return m_nodes[n1].tau <= m_nodes[n2].tau ? n1 : n2;
}

int Graph::getHotterAddr(int n1, int n2)
{
	return m_nodes[n1].tau >= m_nodes[n2].tau ? n1 : n2;
}

void Graph::printAdjList(int nodeAddr)
{
		printf("%5d:od=%-2d,id=%-2d,tau=%-3.1f, avgLoad=%-4d -> [", nodeAddr, m_nodes[nodeAddr].od, m_nodes[nodeAddr].id, m_nodes[nodeAddr].tau, m_nodes[nodeAddr].avgLoad);
		for (list<int>::iterator it = m_adjList[nodeAddr].begin(); it != m_adjList[nodeAddr].end(); it++)
		{
			printf("%5d:%-4.1f", *it, m_nodes[*it].tau);
		}
		printf("]\n");
}

void Graph::saveMToQueue()
{
	// 完成一次Gossip后各个节点保存该轮收到总消息数到缓存队列中
	for (int i = 0; i < m_nV; i++)
	{
		if (m_nodes[i].mq.size() >= mq_len)
		{
			m_nodes[i].mq.pop();
		}
		m_nodes[i].mq.push(m_nodes[i].m);
		m_nodes[i].m = 0;
	}
}

void Graph::calculateTau()
{
	int sum;
	// 计算各个节点冷热度
	for (int i = 0; i < m_nV; i++)
	{
		sum = 0;
		for (int j = 0; j < m_nodes[i].mq.size(); j++)
		{
			sum += m_nodes[i].mq.front();
			m_nodes[i].mq.push(m_nodes[i].mq.front());	// 队列无法遍历，只能不断读取front() push到队尾，然后再pop()掉
			m_nodes[i].mq.pop();
		}
		if (m_nodes[i].mq.size() > 0)
		{
			m_nodes[i].avgLoad = sum / (int)m_nodes[i].mq.size();	// 计算缓存队列平均值
		}
		else
		{
			m_nodes[i].avgLoad = 0;
		}
		m_nodes[i].tau = m_w * m_nodes[i].id + (1 - m_w) * m_nodes[i].avgLoad;	// 按权重计算冷热度
		//m_nodes[i].tau = (m_nodes[i].id + m_nodes[i].avgLoad) / 2;
	}
}


void Graph::BFSUtil(int u, bool visited[]) 		//BFS
{
	queue<int> q;
	q.push(u);
	visited[u] = true;
	list<int>::iterator it;
	while (!q.empty())
	{
		int v = q.front();
		q.pop();
		for (it = m_adjList[v].begin(); it != m_adjList[v].end(); it++)
		{
			if (!visited[*it])
			{
				q.push(*it);
				visited[*it] = true;
			}
		}
	}
}

void Graph::PrintView()
{
	printf("    ");
	for (int i = 0; i < m_nV; i++)
	{
		printf("%-4d", i);
	}
	cout << endl;

	for (int i = 0; i < m_nV; i++)
	{
		printf("%-4d", i);
		for (int j = 0; j < m_nV; j++) {
			if (m_adjMatrix[i][j] == 0) {
				m_adjMatrix[i][j] = 0;
				m_adjMatrix[i][i] = 0;
			}
			printf("%-4d", m_adjMatrix[i][j]);
			//cout << " " << a[i][j] << " ";
		}
		cout << endl;
	}

	for (int i = 0; i < m_nV; i++)
	{
		printAdjList(i);
	}
}




int Graph::GetShortPath(int nodeStart, int nodeEnd)
{
	if (nodeStart == nodeEnd)
	{
		return 0;
	}

	pair<int, int> tipS(nodeStart, 0);
	queue<pair<int, int>> SearchQueue;

	int* MarkList = new int[m_nV];
	memset(MarkList, 0, m_nV * sizeof(int));

	
	SearchQueue.push(tipS);
	while (!SearchQueue.empty())
	{
		pair<int ,int> HandleT = SearchQueue.front();
		SearchQueue.pop();
		MarkList[HandleT.first] = 1;

		for (int index = 0; index < m_nV; ++index)
		{
			if (m_adjMatrix[HandleT.first][index] == 1)
			{
				if (index == nodeEnd)
				{
					delete[] MarkList;
					return HandleT.second + 1;
				}
				else
				{
					if (MarkList[index] == 0)
					{
						SearchQueue.push(pair<int, int>(index, HandleT.second + 1));
					}
					
				}
			}
		}
	}

	
	delete[] MarkList;
	return 0;
}

void Graph::Thread_ShortPathWorker(PSTTHREAD_PARAM param)
{
	while (true)
	{
		if (param->taskqueue->empty())
		{
			break;
		}

		param->mtx_taskqueue->lock();
		pair<int, int> WorkTip = param->taskqueue->front();
		param->taskqueue->pop();
		param->mtx_taskqueue->unlock();

		double PathLen = param->pClass->GetShortPath(WorkTip.first, WorkTip.second);

		param->mtx_TotalPathLen->lock();
		*(param->TotalPathLen) += PathLen;
		param->mtx_TotalPathLen->unlock();

	}

}

double Graph::GetAvPathLen()
{
	double TotalPathLen = 0;

	queue<pair<int, int>> taskqueue;
	for (int index = 0; index < m_nV; ++index)
	{
		for (int i = 0; i < m_nV; ++i)
		{
			taskqueue.push(pair<int, int>(index, i));
		}
	}

	STTHREAD_PARAM param = {0};
	param.mtx_taskqueue = new mutex;
	param.mtx_TotalPathLen = new mutex;
	param.pClass = this;
	param.taskqueue = &taskqueue;
	param.TotalPathLen = &TotalPathLen;

	int threadcount = 20;
	vector<thread *> threadvector;
	for (int i = 0; i < threadcount; ++i)
	{
		thread* pThread = new thread(Thread_ShortPathWorker, &param);
		threadvector.push_back(pThread);
	}

	for (auto threadproc : threadvector)
	{
		threadproc->join();
	}
	


	return TotalPathLen / ((m_nV - 1.0) * m_nV);
}





double Graph::GetCluCoef(int node)
{
	list<int> ParentNode;
	for (int index = 0; index < m_nV; ++index)
	{
		if (m_adjMatrix[node][index] == 1)
		{
			ParentNode.push_back(index);
		}
	}

	double count = 0.0;
	for (auto i = ParentNode.begin(); i != ParentNode.end(); ++i)
	{
		for (auto j = i; j != ParentNode.end(); ++j)
		{
			if (m_adjMatrix[*i][*j] == 1 || m_adjMatrix[*j][*i] == 1)
			{
				count++;
			}
		}
	}

	return count / ((ParentNode.size() -1) * ParentNode.size());
}

double Graph::GetAvCluCoef()
{
	double TotalCount = 0.0;
	for (int index = 0; index < m_nV; ++index)
	{
		TotalCount += GetCluCoef(index);
	}

	return TotalCount / m_nV;
}