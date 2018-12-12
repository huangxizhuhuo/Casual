#include "AStar.h"
#include "ChessBoard.h"
#include <cmath>

CAStar::CAStar()
{
	m_pBest=NULL;
	udCost=NULL;// Called when cost value is need
	udValid=NULL;// Called to check validity of a coordinate
}

void CAStar::StepInitialize(int sx, int sy, int dx, int dy)
{
	ClearNodes();
	m_iSX = sx;//ʼ�� 
	m_iSY = sy; 
	m_iDX = dx; //�յ�
	m_iDY = dy;
	m_iDNum = Coord2Num(dx,dy);//�յ���
	_asNode* temp = new _asNode(sx, sy);//�½��ڵ�
	temp->g = 0;//��ֵ
	temp->h = abs(dx-sx) + abs(dy-sy);
	temp->f = temp->g + temp->h;
	temp->number = Coord2Num(sx, sy);
	m_lOpen.push_front(temp);
}

bool CAStar::GeneratePath(int sx, int sy, int dx, int dy,ChessBoard* chessboard) 
{
	//m_tex=tex;
	m_ChessBoard=chessboard;
	//m_Graphics=graphics;
	StepInitialize(sx, sy, dx, dy);
	int retval = 0;
	while (retval == 0) //����ִ��ֱ���ɹ���ʧ��
	{
		retval = Step();
	};
	
	if (retval == -1 || !m_pBest) 
	{
		m_pBest = NULL;
		return false;//����ʧ�ܣ�δ�ҵ�·��
	}
	//m_Graphics->EndSprite();
	return true;//���سɹ����ҵ�һ������·��
}

int CAStar::Step()
{
	if (!(m_pBest = GetBest()))
		return -1;
	int x=m_ChessBoard->grid1[m_pBest->x][m_pBest->y].x;
	int y=m_ChessBoard->grid1[m_pBest->x][m_pBest->y].y;
	
	//m_tex->Blit(x,y);
		
	if (m_pBest->number == m_iDNum) //n=Ŀ��״̬���ҵ��յ�
		return 1;
	CreateChildren(m_pBest);//���������������������n�����е���״̬
	return 0;
}

_asNode* CAStar::GetBest()//��Open��ɾ����һ��״̬����֮Ϊn�����뵽Closed��
{
	if(m_lOpen.empty())//���Open��Ϊ�գ����˳���û�ҵ�·��
		return NULL;
	_asNode* temp=m_lOpen.front();
	m_lClosed.push_front(temp);
	m_lOpen.pop_front();
	return temp;
}

void CAStar::CreateChildren(_asNode* node) //����node��������״̬
{
	_asNode* temp=new _asNode();;
	int x = node->x, y = node->y;
	for(int i=-1;i<2;i++) //����ͨ����n��ÿ���Ϸ�����״̬���ڴ�Ϊ��·���ߣ����Ҳ�������
	{
		for (int j=-1;j<2;j++) 
		{
			if((i!=0&&j==0)|| i==0&&j!=0)
			{
		    	temp->x = x+i;
			    temp->y = y+j;
			    if (!udFunc(udValid, node, temp, 0)) 
				    continue;
			    LinkChild(node, temp);//��n��ÿ����״̬
			}
		}
	}
}

void CAStar::LinkChild(_asNode* node, _asNode* temp)
{
	int x=temp->x;
	int y=temp->y;
	int g=node->g+udFunc(udCost,node,temp,0);//�ӽڵ��g=���ڵ��g+udCost�������û��Զ�����ۺ���udCost������g��ֵ
	int num=Coord2Num(x,y);// �����ӽڵ�ı��
	_asNode* check=NULL;//��¼���̵�·��
	if(check=CheckList(m_lOpen,num))//���ӽڵ��Ѿ���Open���У�
	{
		node->children[node->numchildren++]=check;//����node��������
		if(g<check->g)//��ʼ�㵽�ýڵ��·�����ȣ�g����С���ı�Open���д˽ڵ��parent��g��f
		{
			check->parent=node;
			check->g=g;
			check->f=g+check->h;
		}
	}
	else if(check=CheckList(m_lClosed,num))//���ӽڵ���Closed���У�
	{
		node->children[node->numchildren++]=check;//����node��������
		if(g<check->g)//����״̬������һ������Closed�������еĸ��̵�·��������
		{//������״̬��Closed��ӵ�Open���У���¼����·����������ۺ���ֵ
			check->parent=node;
			check->g=g;
			check->f=g+check->h;
			UpdateParents(check);//�����������Ľڵ����f��g��h
		}
	}
	else//�Ȳ���Open��Ҳ����Closed�����½�һ�ڵ㣬��������ۺ���ֵ�����뵽Open����
	{
		_asNode* newNode= new _asNode(x,y);
		newNode->parent=node;
		newNode->g=g;
		newNode->h=abs(x-m_iDX)+abs(y-m_iDY);
		newNode->f=newNode->g+newNode->h;
		newNode->number=Coord2Num(x,y);
		AddToOpen(newNode);
		node->children[node->numchildren++]=newNode;
	}
}

void CAStar::UpdateParents(_asNode* node)
{
	int g=node->g;
	int c=node->numchildren;
	_asNode* kid=NULL;
	for(int i=0;i<c;i++)//node�ڵ��Ѹ��¹�����ˣ��������������ܷ�Ľ�g��ֵ
	{//���ڱ��봫�ݱ䶯��Ϣ�����и��¹��Ľڵ㽫����ջ��ȴ��㷨���°벿���ٴε���
		kid=node->children[i];
		if(g+1<kid->g)//ѹ�������ܸĽ�g��ֵ���ӽڵ���ջ
		{
			kid->g=g+1;
			kid->f=kid->g+kid->h;
			kid->parent=node;//
			m_sStack.push(kid);//Push(kid);
		}
	}
	_asNode* parent;
	while(!m_sStack.empty())//�����ӽڵ����������
	{
		parent=m_sStack.top();
		m_sStack.pop();//Pop();
		c=parent->numchildren;
		for(int i=0;i<c;i++)
		{
			kid=parent->children[i];
			if(parent->g+1<kid->g)//
			{
				kid->g=parent->g+udFunc(udCost,parent,kid,0);
				kid->f=kid->g+kid->h;
				kid->parent=parent;
				m_sStack.push(kid);//Push(kid);
			}
		}
	}
}

_asNode* CAStar::CheckList(std::list<_asNode*>& lst, int num)
{
	std::list<_asNode*>::iterator it=lst.begin();
	while(it!=lst.end())
	{
		if((*it)->number==num)////
			return (*it);
		++it;
	}
	return NULL;
}

void CAStar::AddToOpen(_asNode* addnode) 
{
	m_lOpen.push_front(addnode);
	m_lOpen.sort(Comp());
}

CAStar::~CAStar()
{
	ClearNodes();
}

void CAStar::ClearNodes()
{
	m_lOpen.clear();
	m_lClosed.clear();
}

