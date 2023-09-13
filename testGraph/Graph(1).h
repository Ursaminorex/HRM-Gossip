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
	float tau;		// ���ȶ�
	int od;		// ����
	int id;		// ���
	int m;		// һ������gossip�����յ�����Ϣ��
	int avgLoad;	// ƽ�����أ��������ƽ��ֵ
	queue<int> mq;	// ���m�Ļ������
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
	void PrintLoadDistribution();	// ��ӡ���ڵ��4��ƽ������
	void RandCreate(); //�������ͼ, ��������Ķ�
	void AddEdge(int v, int w);
	void RemoveEdge(int v, int w);
	void HRM(int activeNode, int passiveNode);	// ��������㷨
	void DFSUtil(int u, bool visited[]);
	void BFSUtil(int u, bool visited[]);
	Graph* GetTranspose();	// ��ȡ����ͼ
	bool IsSc();		//�����ǲ���ǿ��ͬͼ 
	int StartGossip(int mode = 0);	//0��gossip(),1��gossipWithHRM()
	void JoinPeers(int count);
	void LeavePeers(int count);
	
	static void Thread_ShortPathWorker(PSTTHREAD_PARAM param);

	int GetShortPath(int nodeStart, int nodeEnd);
	double GetAvPathLen();

	double GetCluCoef(int node);
	double GetAvCluCoef();


private:
	int m_nV;	//�ڵ���
	int m_nE;	//����
	int m_maxOd;	//������
	float m_w;	//���Ȩֵ (0,1) �������ȶȼ���
	int m_maxHop;	// gossip�������
	p_node m_nodes;
	int** m_adjMatrix;	//�ڽӾ������ڿ��ٲ���
	list<int>* m_adjList; 	//�ڽӱ����ڿ��ٲ����ɾ��		//������ͼ
	list<int>* m_passiveList;	// ������ͼ

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