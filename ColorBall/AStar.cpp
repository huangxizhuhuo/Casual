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
	m_iSX = sx;//始点 
	m_iSY = sy; 
	m_iDX = dx; //终点
	m_iDY = dy;
	m_iDNum = Coord2Num(dx,dy);//终点编号
	_asNode* temp = new _asNode(sx, sy);//新建节点
	temp->g = 0;//赋值
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
	while (retval == 0) //单步执行直到成功或失败
	{
		retval = Step();
	};
	
	if (retval == -1 || !m_pBest) 
	{
		m_pBest = NULL;
		return false;//返回失败，未找到路径
	}
	//m_Graphics->EndSprite();
	return true;//返回成功，找到一条最优路径
}

int CAStar::Step()
{
	if (!(m_pBest = GetBest()))
		return -1;
	int x=m_ChessBoard->grid1[m_pBest->x][m_pBest->y].x;
	int y=m_ChessBoard->grid1[m_pBest->x][m_pBest->y].y;
	
	//m_tex->Blit(x,y);
		
	if (m_pBest->number == m_iDNum) //n=目的状态，找到终点
		return 1;
	CreateChildren(m_pBest);//继续单步，产生子孙，生成n的所有的子状态
	return 0;
}

_asNode* CAStar::GetBest()//从Open表删除第一个状态，称之为n，加入到Closed表
{
	if(m_lOpen.empty())//如果Open表为空，则退出，没找到路径
		return NULL;
	_asNode* temp=m_lOpen.front();
	m_lClosed.push_front(temp);
	m_lOpen.pop_front();
	return temp;
}

void CAStar::CreateChildren(_asNode* node) //生成node的所有子状态
{
	_asNode* temp=new _asNode();;
	int x = node->x, y = node->y;
	for(int i=-1;i<2;i++) //四连通。对n的每个合法的子状态，在此为有路可走，并且不是自身
	{
		for (int j=-1;j<2;j++) 
		{
			if((i!=0&&j==0)|| i==0&&j!=0)
			{
		    	temp->x = x+i;
			    temp->y = y+j;
			    if (!udFunc(udValid, node, temp, 0)) 
				    continue;
			    LinkChild(node, temp);//对n的每个子状态
			}
		}
	}
}

void CAStar::LinkChild(_asNode* node, _asNode* temp)
{
	int x=temp->x;
	int y=temp->y;
	int g=node->g+udFunc(udCost,node,temp,0);//子节点的g=父节点的g+udCost，调用用户自定义代价函数udCost来计算g的值
	int num=Coord2Num(x,y);// 计算子节点的编号
	_asNode* check=NULL;//记录更短的路径
	if(check=CheckList(m_lOpen,num))//该子节点已经在Open表中，
	{
		node->children[node->numchildren++]=check;//加入node子孙数组
		if(g<check->g)//从始点到该节点的路径长度（g）较小，改变Open表中此节点的parent，g，f
		{
			check->parent=node;
			check->g=g;
			check->f=g+check->h;
		}
	}
	else if(check=CheckList(m_lClosed,num))//该子节点在Closed表中，
	{
		node->children[node->numchildren++]=check;//加入node子孙数组
		if(g<check->g)//该子状态是沿着一条比在Closed表中已有的更短的路径而到达
		{//将该子状态从Closed表加到Open表中，记录更短路径走向及其估价函数值
			check->parent=node;
			check->g=g;
			check->f=g+check->h;
			UpdateParents(check);//对所有相连的节点更新f，g，h
		}
	}
	else//既不在Open表也不在Closed表，则新建一节点，计算其估价函数值，加入到Open表中
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
	for(int i=0;i<c;i++)//node节点已更新过，因此，检查所有子孙，看能否改进g的值
	{//由于必须传递变动信息，所有更新过的节点将放在栈里等待算法的下半部分再次调出
		kid=node->children[i];
		if(g+1<kid->g)//压入所有能改进g的值的子节点入栈
		{
			kid->g=g+1;
			kid->f=kid->g+kid->h;
			kid->parent=node;//
			m_sStack.push(kid);//Push(kid);
		}
	}
	_asNode* parent;
	while(!m_sStack.empty())//处理子节点的所有子孙
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

