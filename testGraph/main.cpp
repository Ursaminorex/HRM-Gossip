#include "Graph.h"
#include <stdio.h>
#include <windows.h>

using namespace std;

int nV, nE;    //  m表示结点个数，n表示边的条数  
int max_od;	// od表示最大出度
float w;	// w表示入度权值(0,1)，用于计算冷热度
int mode;	//0是gossip(),1是gossipWithHRM()
int maxHop;	//Gossip最大跳数

Graph* g;

void clearmap()//初始化
{
	if (g)
	{
		delete g;
	}
}

int main()
{
	srand((unsigned int)time(NULL));//按照当前的时间值种下随机种子数，使得任意时刻产生的随机数都是不同的 
	ios::sync_with_stdio(0);
	cin.tie(NULL);
	cout.tie(NULL);
retry:
	cout << "请输入节点的个数：";//最后输出空格 
	cin >> nV;
	cout << "请输入边的个数：";
	cin >> nE;
	while (nE > (nV * (nV - 1))) //判断边数是否输入正确，若要满足有向图定义即  n<m*(m-1) ,无向图即要满足n <m*(m-1)/2
	{
		cout << "输入边数过大，请重新输入：" << endl;
		goto retry;
	}

	cout << "请输入最大出度：";
	cin >> max_od;
	while (nV > nE * max_od) // nV<=nE*max_od
	{
		cout << "输入度数过小，请重新输入：" << endl;
		goto retry;
	}

	//cout << "请输入入度权值(0,1)：";
	//cin >> w;
	cout << "请输入Gossip最大跳数：";
	cin >> maxHop;
	cout << "请输入Gossip模式，0是不优化模式,1表示HRM优化模式：";
	cin >> mode;

	bool isSC = false;
	while (!isSC)
	{
		clearmap();
		g = new Graph(nV, nE, max_od);
		//g->SetWeight(w);
		g->SetMaxHop(maxHop);
		g->SetMode(mode);
		g->RandCreate();	// 随机网络
		//g->PrintView();
		isSC = g->IsSc();
		if (isSC)
		{
			cout << "随机生成的图是一个强连通图！" << endl;
			g->PrintPerformance();
			break;
		}
		else
		{
			cout << "随机生成的图不是强连通图！" << endl;
		}
		string yn;
		cout << "是否需要重新初始化？[Y/N]" << endl;
		cin >> yn;
		if (!yn.compare("y") || !yn.compare("Y"))
		{
			goto retry;
		}
		else
		{
			continue;
		}
	}

	//int activePeer;
	//int passivePeer;
	//activePeer = rand() % nV;	// 随机选择一个主动节点
	// 邻居域调整
	char ch;
	ch = getchar();	// 会自动获取到一次'\n'字符，消耗该字符

	int hop, round = 0;
	//for (int i = 0; i < mq_len; i++)
	//{
	//	while (getchar() != '\n');
	//	hop = g->StartGossip();
	//	++round;
	//	printf("gossip round = %d, hop count = %d\n", round, hop);
	//}
	//while (getchar() != '\n');
	//hop = g->StartGossip(0, 1);
	//++round;
	//printf("hop count = %d\n", hop);
	//g->PrintView();
	//printf("gossip round = %d, hop count = %d\n", round, hop);
	//	g->WriteLoadDistribution(round);
	int countfor10000 = 0;
	//goto single;
	while (g->GetVnum() < 10001)
	{
		hop = g->StartGossip(mode, 0);
		++round;
		//g->PrintView();
		//g->PrintPerformance();
		if (round > 0)
		{
			g->WriteGossipTotalTrend(round, hop);
			g->WriteInDgreeDistribution(round);
			g->WriteLoadDistribution(round);
			g->WriteAvCluCoef(round, g->GetVnum());
		}
		printf("gossip round = %d, hop count = %d, m_nV = %d\n", round, hop, g->GetVnum());

		if (mode)
		{
			isSC = g->IsSc();
			if (isSC)
			{
				cout << "HRM置换后是强连通图！" << endl;
				//Sleep(2000);
				if (g->GetVnum() == 10000)
				{
					if (++countfor10000 == 10)
					{
						//printf("average shortest path length = %f\n", g->GetAvPathLen());
						break;
					}
				}
				if (round % 10 == 0)
				{
					if (2* g->GetVnum() > 10000)
					{
						g->JoinPeers(10000 - g->GetVnum());
						//g->PrintPerformance();
						g->WriteGossipTotalTrend(round, hop);
						g->WriteInDgreeDistribution(round);
						g->WriteLoadDistribution(round);
					}
					else
					{
						g->JoinPeers(g->GetVnum());
						//g->PrintPerformance();
						g->WriteGossipTotalTrend(round, hop);
						g->WriteInDgreeDistribution(round);
						g->WriteLoadDistribution(round);
					}
				}
			}
			else
			{
				cout << "HRM置换后不是强连通图！" << endl;
			}
		}
	}

single:
	while ((ch = getchar()) == '\n' || ch == '1' || ch == '2' || ch == 'r')
	{
		if (ch == '\n')
		{
			////单次gossip分发置换
			//passivePeer = rand() % nV;	// 随机选择一个被动节点
			//while (activePeer == passivePeer)
			//	passivePeer = rand() % nV;	//这两个随机数必须是不相等 
			//g->HRM(activePeer, passivePeer);
			hop = g->StartGossip(mode,0);
			++round;
			g->PrintView();
			g->PrintPerformance();
			//if (round > 0)
			//{
			//	//g->WriteGossipTotalTrend(round, hop);
			//	g->WriteInDgreeDistribution(round);
			//	g->WriteLoadDistribution(round);
			//}
			printf("gossip round = %d, hop count = %d, m_nV = %d\n", round, hop, g->GetVnum());

			if (mode)
			{
				isSC = g->IsSc();
				if (isSC)
				{
					cout << "HRM置换后是强连通图！" << endl;
					//Sleep(2000);
					//g->JoinPeers(g->GetVnum());
					g->PrintPerformance();
					//if (round % 4 == 0)
					//{
					//	g->WriteGossipTotalTrend(round, hop);
					//	g->WriteInDgreeDistribution(round);
					//	g->WriteLoadDistribution(round);
					//}
				}
				else
				{
					cout << "HRM置换后不是强连通图！" << endl;
				}
			}

			//activePeer = passivePeer;
		}
		else if (ch == '1')
		{
			int count;
			cout << "请输入增加节点的个数：";//最后输出空格 
			cin >> count;
			ch = getchar();	// 会自动获取到一次'\n'字符，消耗该字符
			g->JoinPeers(count);
			g->PrintPerformance();
			//if (round > 0)
			//{
			//	//g->WriteGossipTotalTrend(round, hop);
			//	g->WriteInDgreeDistribution(round);
			//	g->WriteLoadDistribution(round);
			//}
		}
		else if (ch == '2')
		{
			int count;
			cout << "请输入删除节点的个数：";//最后输出空格 
			cin >> count;
			ch = getchar();	// 会自动获取到一次'\n'字符，消耗该字符
			g->LeavePeers(count);
			isSC = g->IsSc();
			if (isSC)
			{
				cout << "节点掉线后依然是强连通图！" << endl;
				g->PrintPerformance();
			}
			else
			{
				cout << "节点掉线后不是强连通图！" << endl;
			}
		}
		else if (ch == 'r')
		{
			goto retry;
		}
		cout << "[回车]进入下一轮Gossip" << endl;
		cout << "[1]增加新节点" << endl;
		cout << "[2]删除末尾节点" << endl;
		cout << "[r]重新构建随机网络" << endl;
	}

	for (int i = 0; i < nV; i++)
	{
		// i-> (nV-1) 最短路径
		// (nV-1)->i最短路径
	}
	//avg

	clearmap();
	system("pause");
	return 0;
}