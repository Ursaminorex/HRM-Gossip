#include<iostream>
#include<list> 
#include<queue>
#include <ctime>  
#include <cstdlib>  
#include <cstring>  
#include <iomanip>  
#include <vector>
#include <queue>
#include <mutex>
#include <thread>

//#define MAX 1000
const int mq_len = 4;

using namespace std;

typedef struct Node {
	float tau;		// 冷热度
	int od;		// 出度
	int id;		// 入度
	int m;		// 一次完整gossip迭代收到的消息数
	int avgLoad;	// 平均负载，缓冲队列平均值
	queue<int> mq;	// 存放m的缓冲队列
	Node()
	{
		tau = 0;
		od = 0;
		id = 0;
		m = 0;
		avgLoad = 0;
	}
}*p_node;


class Graph;

typedef struct
{
	double* TotalPathLen;
	queue<pair<int, int>>* taskqueue;
	Graph* pClass;
	mutex* mtx_taskqueue;
	mutex* mtx_TotalPathLen;
} STTHREAD_PARAM, *PSTTHREAD_PARAM;


class Graph
{
public:
	Graph(int _nV, int _nE, int _maxOd);
	~Graph();
	void SetWeight(float _w);
	void SetMaxHop(int _maxhop);
	int GetVnum();
	void PrintView();
	void PrintLoadDistribution();	// 打印各节点近4轮平均负载
	void RandCreate(); //生成随机图, 计算各结点的度
	void AddEdge(int v, int w);
	void RemoveEdge(int v, int w);
	void HRM(int activeNode, int passiveNode);	// 文章提出算法
	void DFSUtil(int u, bool visited[]);
	void BFSUtil(int u, bool visited[]);
	Graph* GetTranspose();	// 获取反向图
	bool IsSc();		//返回是不是强连同图 
	int StartGossip(int mode = 0);	//0是gossip(),1是gossipWithHRM()
	void JoinPeers(int count);
	void LeavePeers(int count);
	
	static void Thread_ShortPathWorker(PSTTHREAD_PARAM param);

	int GetShortPath(int nodeStart, int nodeEnd);
	double GetAvPathLen();

	double GetCluCoef(int node);
	double GetAvCluCoef();


private:
	int m_nV;	//节点数
	int m_nE;	//边数
	int m_maxOd;	//最大出度
	float m_w;	//入度权值 (0,1) 用于冷热度计算
	int m_maxHop;	// gossip最大跳数
	p_node m_nodes;
	int** m_adjMatrix;	//邻接矩阵，用于快速查找
	list<int>* m_adjList; 	//邻接表，用于快速插入和删除		//主动视图
	list<int>* m_passiveList;	// 被动视图

	int getColdestNeighborAddr(int nodeAddr);
	int getHottestNeighborAddr(int nodeAddr);
	int getSecondLastColdNeighborAddr(int nodeAddr, int coldestAddr);
	int getColderAddr(int n1, int n2);
	int getHotterAddr(int n1, int n2);
	void printAdjList(int nodeAddr);
	int gossip(int activePeer, bool received[], int maxHop);
	int gossipWithHRM(int activePeer, bool received[], int maxHop);
	void saveMToQueue();
	void calculateTau();
	

	


};