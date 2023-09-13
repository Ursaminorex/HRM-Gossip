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
	float tau;		// ���ȶ�
	int od;		// ����
	int id;		// ���
	int m;		// һ������gossip�����յ�����Ϣ��
	int avgLoad;	// ƽ�����أ��������ƽ��ֵ
	double cluCoef;	// �ۼ�ϵ��
	queue<int> mq;	// ���m�Ļ������
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
	void PrintPerformance();	// ��ӡ����ָ��
	void RandCreate(); //�������ͼ, ��������Ķ�
	void AddEdge(int v, int w);
	void RemoveEdge(int v, int w);
	void HRM(int activeNode, int passiveNode);	// ��������㷨
	void DFSUtil(int u, bool visited[]);
	void BFSUtil(int u, bool visited[]);
	Graph* GetTranspose();	// ��ȡ����ͼ
	bool IsSc();		//�����ǲ���ǿ��ͬͼ 
	int StartGossip(int mode, int flag);	//0��gossip(),1��gossipWithHRM()
	void JoinPeers(int count);
	void LeavePeers(int count);
	void WriteLoadDistribution(int round);	// ����ڵ�ƽ�����طֲ�
	void WriteInDgreeDistribution(int round);	// ����ڵ���ȷֲ�
	void WriteGossipTotalTrend(int round, int total);	// ����Gossip����������Ϣ����
	void WriteAvCluCoef(int round, int num);	// ����ƽ���ۼ�ϵ��

	int GetShortPath(int nodeStart, int nodeEnd);
	double GetAvPathLen();
	double GetAvCluCoef();
	double GetMaxCluCoef();
	double GetMinCluCoef();

	int GetMaxAvgLoad();
	int GetMaxInDgree();

private:
	int m_nV;	//�ڵ���
	int m_nE;	//����
	int m_maxOd;	//������
	int m_maxHop;	// gossip�������
	int m_mode;	// 0����ͨģʽ��1��HRM�Ż�ģʽ
	p_node m_nodes;
	int** m_adjMatrix;	//�ڽӾ������ڿ��ٲ���
	list<int>* m_adjList; 	//�ڽӱ����ڿ��ٲ����ɾ��		//������ͼ
	list<int>* m_passiveList;	// ������ͼ

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