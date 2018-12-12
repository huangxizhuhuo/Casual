// Class:	CAStar class (27/6/2001)
// Author:	James Matthews
#pragma once

#include <memory>
#include <list>
#include <stack>

struct ChessBoard;
struct _asNode 
{		
	_asNode(int a = -1,int b = -1) : x(a), y(b), number(0), numchildren(0) 
	{			
		parent = NULL; 		
		memset(children, 0, sizeof(children));		
	}
	int		 f,g,h;			// Fitness, goal, heuristic.
	int		 x,y;			// Coordinate position
	int		 numchildren;
	int		 number;		// x*m_iRows+y
	_asNode* parent;
	_asNode* children[4];	// Assumes square tiles
};

typedef int(*_asFunc)(_asNode*, _asNode*, int);

struct Comp
{
	bool operator()(_asNode* _Left,_asNode* _Right) const
	{
		return _Left->f<_Right->f;
	}
};

class CAStar 
{	
public:	
	CAStar();
	~CAStar();
	_asFunc	 udCost;			// Called when cost value is need
	_asFunc  udValid;			// Called to check validity of a coordinate	
	bool	 GeneratePath(int, int, int, int,ChessBoard*);
	int		 Step();
	void	 StepInitialize(int, int, int, int);
	void	 SetRows(int r)	{ m_iRows = r; }
	void	 Reset() { m_pBest=NULL; }
	_asNode* GetBestNode() { return m_pBest; }
protected:
	int		m_iRows;			// Used to calculate node->number
	int		m_iSX, m_iSY, m_iDX, m_iDY, m_iDNum;
	//CTexture* m_tex;
	ChessBoard* m_ChessBoard;
	//CGraphics* m_Graphics;
	std::list<_asNode*> m_lOpen;
	std::list<_asNode*> m_lClosed;
	std::stack<_asNode*> m_sStack;
	_asNode* m_pBest;
	void	AddToOpen(_asNode*);
	void	ClearNodes();
	void	CreateChildren(_asNode*);
	void	LinkChild(_asNode*, _asNode*);
	void	UpdateParents(_asNode*);
	_asNode* CheckList(std::list<_asNode*>&, int);//const std::list<_asNode*>& wrong
	_asNode* GetBest();	
	inline int Coord2Num(int x, int y) { return x * m_iRows + y; }
	inline int udFunc(_asFunc func, _asNode* param1, _asNode* param2, int data)
	{
		if (func) 	
			return func(param1, param2, data);
		return 1;
	}
};
