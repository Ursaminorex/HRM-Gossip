#include<iostream>
#include<list> 
#include <ctime>  
#include <cstdlib>  
#include <cstring>  
#include <iomanip>  
#include <vector>
#include <queue>
//#include <stdio.h>

#define MAXPATH 260
//#define MAX 1000
const int mq_len = 4;

using namespace std;

typedef struct Node {
	float tau;		// 冷热度
	int od;		// 出度
	int id;		// 入度
	int m;		// 一次完整gossip迭代收到的消息数
	int avgLoad;	// 平均负载，缓冲队列平均值
	double cluCoef;	// 聚集系数
	queue<int> mq;	// 存放m的缓冲队列
	Node()
	{
		tau = 0;
		od = 0;
		id = 0;
		m = 0;
		avgLoad = 0;
		cluCoef = 0.0;
	}
}*p_node;

class Graph
{
public:
	Graph(int _nV, int _nE, int _maxOd);
	~Graph();
	void SetMaxHop(int _maxhop);
	void SetMode(int _mode);
	int GetVnum();
	void PrintView();
	void PrintPerformance();	// 打印性能指标
	void RandCreate(); //生成随机图, 计算各结点的度
	void AddEdge(int v, int w);
	void RemoveEdge(int v, int w);
	void HRM(int activeNode, int passiveNode);	// 文章提出算法
	void DFSUtil(int u, bool visited[]);
	void BFSUtil(int u, bool visited[]);
	Graph* GetTranspose();	// 获取反向图
	bool IsSc();		//返回是不是强连同图 
	int StartGossip(int mode, int flag);	//0是gossip(),1是gossipWithHRM()
	void JoinPeers(int count);
	void LeavePeers(int count);
	void WriteLoadDistribution(int round);	// 保存节点平均负载分布
	void WriteInDgreeDistribution(int round);	// 保存节点入度分布
	void WriteGossipTotalTrend(int round, int total);	// 保存Gossip完整迭代消息总量
	void WriteAvCluCoef(int round, int num);	// 保存平均聚集系数

	int GetShortPath(int nodeStart, int nodeEnd);
	double GetAvPathLen();
	double GetAvCluCoef();
	double GetMaxCluCoef();
	double GetMinCluCoef();

	int GetMaxAvgLoad();
	int GetMaxInDgree();

private:
	int m_nV;	//节点数
	int m_nE;	//边数
	int m_maxOd;	//最大出度
	int m_maxHop;	// gossip最大跳数
	int m_mode;	// 0是普通模式，1是HRM优化模式
	p_node m_nodes;
	int** m_adjMatrix;	//邻接矩阵，用于快速查找
	list<int>* m_adjList; 	//邻接表，用于快速插入和删除		//主动视图
	list<int>* m_passiveList;	// 被动视图

	int getColdestNeighborAddr(int nodeAddr);
	int getHottestNeighborAddr(int nodeAddr);
	int getSecondLastColdNeighborAddr(int nodeAddr, int coldestAddr);
	int getColderAddr(int n1, int n2);
	int getHotterAddr(int n1, int n2);
	double getCluCoef(int node);
	void printAdjList(int nodeAddr);
	int gossip(int activePeer, bool received[], int maxHop, int flag);
	int gossipWithHRM(int activePeer, bool received[], int maxHop, int flag);
	void saveMToQueue();
	void calculateTau();
	void calculateCluCoef();


};