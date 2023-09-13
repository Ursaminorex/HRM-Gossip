#include "Graph.h"
#include <stdio.h>
#include <windows.h>

using namespace std;

int nV, nE;    //  m��ʾ��������n��ʾ�ߵ�����  
int max_od;	// od��ʾ������
float w;	// w��ʾ���Ȩֵ(0,1)�����ڼ������ȶ�
int mode;	//0��gossip(),1��gossipWithHRM()
int maxHop;	//Gossip�������

Graph* g;

void clearmap()//��ʼ��
{
	if (g)
	{
		delete g;
	}
}

int main()
{
	srand((unsigned int)time(NULL));//���յ�ǰ��ʱ��ֵ���������������ʹ������ʱ�̲�������������ǲ�ͬ�� 
	ios::sync_with_stdio(0);
	cin.tie(NULL);
	cout.tie(NULL);
retry:
	cout << "������ڵ�ĸ�����";//�������ո� 
	cin >> nV;
	cout << "������ߵĸ�����";
	cin >> nE;
	while (nE > (nV * (nV - 1))) //�жϱ����Ƿ�������ȷ����Ҫ��������ͼ���弴  n<m*(m-1) ,����ͼ��Ҫ����n <m*(m-1)/2
	{
		cout << "��������������������룺" << endl;
		goto retry;
	}

	cout << "�����������ȣ�";
	cin >> max_od;
	while (nV > nE * max_od) // nV<=nE*max_od
	{
		cout << "���������С�����������룺" << endl;
		goto retry;
	}

	//cout << "���������Ȩֵ(0,1)��";
	//cin >> w;
	cout << "������Gossip���������";
	cin >> maxHop;
	cout << "������Gossipģʽ��0�ǲ��Ż�ģʽ,1��ʾHRM�Ż�ģʽ��";
	cin >> mode;

	bool isSC = false;
	while (!isSC)
	{
		clearmap();
		g = new Graph(nV, nE, max_od);
		//g->SetWeight(w);
		g->SetMaxHop(maxHop);
		g->SetMode(mode);
		g->RandCreate();	// �������
		//g->PrintView();
		isSC = g->IsSc();
		if (isSC)
		{
			cout << "������ɵ�ͼ��һ��ǿ��ͨͼ��" << endl;
			g->PrintPerformance();
			break;
		}
		else
		{
			cout << "������ɵ�ͼ����ǿ��ͨͼ��" << endl;
		}
		string yn;
		cout << "�Ƿ���Ҫ���³�ʼ����[Y/N]" << endl;
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
	//activePeer = rand() % nV;	// ���ѡ��һ�������ڵ�
	// �ھ������
	char ch;
	ch = getchar();	// ���Զ���ȡ��һ��'\n'�ַ������ĸ��ַ�

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
				cout << "HRM�û�����ǿ��ͨͼ��" << endl;
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
				cout << "HRM�û�����ǿ��ͨͼ��" << endl;
			}
		}
	}

single:
	while ((ch = getchar()) == '\n' || ch == '1' || ch == '2' || ch == 'r')
	{
		if (ch == '\n')
		{
			////����gossip�ַ��û�
			//passivePeer = rand() % nV;	// ���ѡ��һ�������ڵ�
			//while (activePeer == passivePeer)
			//	passivePeer = rand() % nV;	//����������������ǲ���� 
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
					cout << "HRM�û�����ǿ��ͨͼ��" << endl;
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
					cout << "HRM�û�����ǿ��ͨͼ��" << endl;
				}
			}

			//activePeer = passivePeer;
		}
		else if (ch == '1')
		{
			int count;
			cout << "���������ӽڵ�ĸ�����";//�������ո� 
			cin >> count;
			ch = getchar();	// ���Զ���ȡ��һ��'\n'�ַ������ĸ��ַ�
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
			cout << "������ɾ���ڵ�ĸ�����";//�������ո� 
			cin >> count;
			ch = getchar();	// ���Զ���ȡ��һ��'\n'�ַ������ĸ��ַ�
			g->LeavePeers(count);
			isSC = g->IsSc();
			if (isSC)
			{
				cout << "�ڵ���ߺ���Ȼ��ǿ��ͨͼ��" << endl;
				g->PrintPerformance();
			}
			else
			{
				cout << "�ڵ���ߺ���ǿ��ͨͼ��" << endl;
			}
		}
		else if (ch == 'r')
		{
			goto retry;
		}
		cout << "[�س�]������һ��Gossip" << endl;
		cout << "[1]�����½ڵ�" << endl;
		cout << "[2]ɾ��ĩβ�ڵ�" << endl;
		cout << "[r]���¹����������" << endl;
	}

	for (int i = 0; i < nV; i++)
	{
		// i-> (nV-1) ���·��
		// (nV-1)->i���·��
	}
	//avg

	clearmap();
	system("pause");
	return 0;
}