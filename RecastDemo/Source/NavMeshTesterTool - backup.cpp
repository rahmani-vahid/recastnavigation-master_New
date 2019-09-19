//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#define _USE_MATH_DEFINES
#include <math.h>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include <glut.h>
#include "imgui.h"
#include "NavMeshTesterTool.h"
#include "Sample.h"
#include "Recast.h"
#include "RecastDebugDraw.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourDebugDraw.h"
#include "DetourCommon.h"
#include "DetourAssert.h"
#include <windows.h>
#include <string> 
#include <sstream>
#ifdef WIN32
#	define snprintf _snprintf
#endif

// Uncomment this to dump all the requests in stdout.
#define DUMP_REQS
using namespace std;
const int totalNodes = 20000;
vector<int> adj[totalNodes];
int Hexcounter[totalNodes][2];
float hexNode[totalNodes][6];

int f = 0;
const int hyy = 0; // z
// Returns a random number [0..1)
static float frand()
{
	//	return ((float)(rand() & 0xffff)/(float)0xffff);
	return (float)rand() / (float)RAND_MAX;
}


//====================================

void DFSUtil(int n, bool visited[], bool f, int Hexcounter[totalNodes][2])
{
	// Mark the current node as visited and 
	// print it 

	std::ofstream fout("DFS.txt", std::ios_base::app | std::ios_base::out);
	
	
	
	if (f == true){
		bool flag = true;
		visited[n] = true;
		Hexcounter[n][1] = Hexcounter[n][1] + 1;

		fout << n << " ";
		if (n == 1505)
			flag = false;

		float childNode[3];
		float openedNode[3];
		int bestNode;
		openedNode[0] = hexNode[1505][2];
		openedNode[1] = 0;
		openedNode[2] = hexNode[1505][4];


		int max = 1000000;

		// Recur for all the vertices adjacent to this vertex 
		vector<int>::iterator i;
		for (i = adj[n].begin(); i != adj[n].end(); ++i){

			
			
			childNode[0] = hexNode[*i][2];
			childNode[1] = 0;
			childNode[2] = hexNode[*i][4];

			float dis = dtVdist(childNode, openedNode);
		

			Hexcounter[*i][1] = Hexcounter[*i][1] + 1;


			if (dis < max){
				max = dis;
				bestNode = *i;
			}

		}
		
		Hexcounter[bestNode][1] = Hexcounter[bestNode][1] + 1;
			if (!visited[bestNode])
			DFSUtil(bestNode, visited, flag, Hexcounter);
		}
	
}



void DFS(int v)
{
	// Mark all the vertices as not visited 
	bool *visited = new bool[totalNodes];
	for (int i = 0; i < totalNodes; i++){
		visited[i] = false;
		Hexcounter[i][0] = i;
		//Hexcounter[i][1] = 0;
	}
	// Call the recursive helper function 
	// to print DFS traversal 
	DFSUtil(v, visited, true, Hexcounter);
}










std::vector<int> parse_line(std::string line)
{
	std::istringstream stm(line);
	return{ std::istream_iterator<int>(stm), std::istream_iterator<int>() };
}

std::vector< std::vector<int> > read_lines(std::istream& stm)
{
	std::vector< std::vector<int> > result;

	std::string line;
	while (std::getline(stm, line)) result.push_back(parse_line(line));

	return result;
}


//======================================
void myinit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-80, 80, -80, 80);
}

//void lineSegment(float p1, float p11, float p2, float p22, float p3, float p33, float p4, float p44, float p5, float p55, float p6, float p66)
void lineSegment()

{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.4, 0.2);
	int maxCounter = 8;
	int minCounter = 0;



	float y1, y2, y3, y4, y5, y6,y7;
	int id, id1, id2, id3, id4, id5, id6,id7;
	float p1, p11, p2, p22, p3, p33, p4, p44, p5, p55, p6, p66,p77,p7;
	/**/
	std::ifstream s("graph.txt");
	while (s >> id1 >> p1 >> y1 >> p11 >>
		id2 >> p2 >> y2 >> p22 >>
		id3 >> p3 >> y3 >> p33 >>
		id4 >> p4 >> y4 >> p44 >>
		id5 >> p5 >> y5 >> p55 >>
		id6 >> p6 >> y6 >> p66>>
		id7>>p7>>y7>>p77){

		//float c0 = 1/id2;
		//float c1 = 1/id2+50;
		//float c2= 1/id2+100;
		
		

		int s = 1.5;
		
		float pi1[] = { p3*s, -p33*s };
		float pi2[] = { p4*s, -p44*s };
		float pi3[] = { p2*s, -p22*s };
		float pi4[] = { p6*s, -p66*s };
		float pi5[] = { p5*s, -p55*s };
		float pi6[] = { p1*s, -p11*s };


		glColor3ub(0.0,0.0,0.0);


		glBegin(GL_LINES);
		glVertex2fv(pi1);
		glVertex2fv(pi2);
		glEnd();



		glBegin(GL_LINES);
		glVertex2fv(pi2);
		glVertex2fv(pi3);
		glEnd();


		glBegin(GL_LINES);
		glVertex2fv(pi3);
		glVertex2fv(pi4);
		glEnd();


		glBegin(GL_LINES);
		glVertex2fv(pi4);
		glVertex2fv(pi5);
		glEnd();


		glBegin(GL_LINES);
		glVertex2fv(pi5);
		glVertex2fv(pi6);
		glEnd();





		glBegin(GL_LINES);
		glVertex2fv(pi6);
		glVertex2fv(pi1);
		glEnd();


		



		int a = Hexcounter[id2][1];
		
	//	int range = maxCounter - minCounter;
		//a = (a - minCounter) / range;

		//int range2 = 255 - 0;
	//	a =round( (a * range2)) + 0;

		 
if (a==0)
		glColor3ub(204, 255, 255);

if (a == 3)
glColor3ub(204, 255, 255);

if (a == 1)
glColor3ub(70, 255, 255);

if (a == 2)
glColor3ub(120, 255, 255);

if (a == 4)
glColor3ub(0, 128, 255);

if (a == 5)
glColor3ub(102, 102, 255);

if (a == 6)
glColor3ub(127, 0, 255);
if (a == 7)
glColor3ub(102, 0, 204);
if (a == 8)
glColor3ub(0, 0, 153);

if (id2==1505)
glColor3ub(0, 0, 0);

if (id2 == 1057)
glColor3ub(0, 0, 0);


		glBegin(GL_POLYGON);
		glVertex2fv(pi1);
		glVertex2fv(pi2);
		glVertex2fv(pi3);
		glVertex2fv(pi4);
		glVertex2fv(pi5);
		glVertex2fv(pi6);
		glEnd();

		glFlush();
	}



	 
	
}

//=============================GPS PARAMETERS =============================


float size=3;
 float w=4.5;   // (2 * size) *(3 / 4);  // ==> 1.5   for siz=1   and 4.5 for size =3 
 float blueStep=1.5;  // size / 2;
 int step=size*3;
//======================================================


class voronoi{
	float pos[3];
	float edge_pos[3][6];
	int counter;
	int id;
	int polyID;
	voronoi *next;
	friend class linkedlist;
};
//--------------


class linkedlist{
public:
	linkedlist(){ first = NULL; }
	void addelement(float, float, float, int, int, int);//add node to list
	//void deleteelement(int);//delet node by value of data
	void showlist();//print all node in list
	//void search_data(int);//search the linked list by value of data
	void add_counter(int id);
	void show_counter();
	void update();

private:
	voronoi *first;//stor first node of link
	voronoi *last;//stor last node of link
}a;


//--------

void linkedlist::update(){

}
//
 
void linkedlist::add_counter(int id){

	voronoi *temp;
	temp = first;
	while (temp != first)
	{
		if (temp->id == id){
			temp->counter = temp->counter + 1;
		
		}
		temp = temp->next;

	}

}


void linkedlist::addelement(float x, float y, float z, int counter, int id, int polyID)
{
	//int data1;
	if (first == NULL)
	{
		//create first node

		voronoi *temp = new voronoi;
		temp->next = NULL;
		first = temp;
		first->pos[0] = x;
		first->pos[1] = y;
		first->pos[2] = z;
		first->counter = counter;
		first->id = id;
		first->polyID = polyID;
		last = first;
	}
	else
	{
		//create other node

		voronoi *temp = new voronoi;
		temp->next = NULL;
		last->next = temp;
		last = temp;
		last->pos[0] = x;
		last->pos[1] = y;
		last->pos[2] = z;
		last->counter = counter;
		last->id = id;
		first->polyID = polyID;
	}
}
//----------------

void linkedlist::showlist()
{
	float c[3], g[3];
	float x, y, z;
	float xx, yy, zz, dis3;
	int id, idd, gpsID, cc;
	int f = 0;
	float dis2 = 10000000000;
	float pathid[3];
	dtPolyRef re;
	float xxx, yyy, zzz;
	float Y;

	std::ofstream fout("gpsID.txt", std::ios_base::app | std::ios_base::out);

	//std::ofstream fout("GPS.txt", std::ios_base::app | std::ios_base::out);

	if (first == NULL)
	{
		fout << "NULL";
	}
	else
	{
		voronoi *temp;
		temp = first;
		id = 0;
		while (temp != NULL)
		{
		 
			g[0] = temp->pos[0];
			g[1] = 0;
			g[2] = temp->pos[2];
			dis2 = 100000000;
			std::ifstream in4("v1_Center Of polygons2.txt");
			while (in4 >> idd >> xx >> yy >> zz)  {

				c[0] = xx;
				c[1] = 0;
				c[2] = zz;

				dis3 = dtVdist(c, g);
				if (dis3 <= dis2){
					dis2 = dis3;
					gpsID = idd;
					//xxx = c[0];
					//yyy = c[1];
					//zzz = c[2];
					Y = yy;
				}
			}
			//_______________________________________________________________________________________________

			in4.close();
			temp->polyID = gpsID;
			temp->pos[1] = Y;
			//fout << id << "   " << gpsID << "  " << x << "   " << Y << "  " << z << endl;// << " || " << xxx << " " << yyy << " " << zzz << " ||" << dis2 << endl;
			 fout << "\n" << temp->id << "     " << temp->polyID<<"     " << temp->pos[0] << "  " <<  /*hyy */ temp->pos[1] << "  " << temp->pos[2] << "  " << temp->counter;
			temp = temp->next;
		}
	}
	fout.close();


}



void addEdge(vector<int> adj[], int node, int child)
{
	adj[node].push_back(child);
	//adj[v].push_back(u);
}




// this test
//vahid
//vahid2

void linkedlist::show_counter()
{
	std::ofstream fout("counter.txt", std::ios_base::app | std::ios_base::out);
	
	if (first == NULL)
	{
		fout << "NULL";
	}
	else
	{
		voronoi *temp;
		temp = first;

		while (temp != NULL)
		{
			if (temp->counter>0)
			fout << "\n" << temp->id << "     " << temp->counter;
			temp = temp->next;
		}
	}
	fout.close();
}


//=====================================



inline bool inRange(const float* v1, const float* v2, const float r, const float h)
{
	const float dx = v2[0] - v1[0];
	const float dy = v2[1] - v1[1];
	const float dz = v2[2] - v1[2];
	return (dx*dx + dz*dz) < r*r && fabsf(dy) < h;
}




//===================

void GPS_PRINT(float x, float y, float z){

	DebugDrawGL dd;

	float pp[3];


	//==========================================
	/*
	dd.vertex(x + size, 0, z, duRGBA(0, 0, 255, 192));   // 1
	dd.vertex(x - size, 0, z, duRGBA(0, 0, 255, 192));  //2

	dd.vertex(x + blueStep, 0, z + size, duRGBA(0, 0, 255, 192));  //3
	dd.vertex(x - blueStep, 0, z + size, duRGBA(0, 0, 255, 192));  //4

	dd.vertex(x + blueStep, 0, z - size, duRGBA(0, 0, 255, 192));  //5
	dd.vertex(x - blueStep, 0, z - size, duRGBA(0, 0, 255, 192));   //6

	*/
	//=============================================









	//===============ok ============================================================================


	dd.begin(DU_DRAW_LINES);
	{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x + size, y, z, duRGBA(176, 76, 82, 255));   // 1
		dd.vertex(x + blueStep, y, z + size, duRGBA(176, 76, 82, 255));  //3
		dd.end();
	}


	dd.begin(DU_DRAW_LINES);
	{
		//	dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x + blueStep, y, z + size, duRGBA(176, 76, 82, 255));  //3
		dd.vertex(x - blueStep, y, z + size, duRGBA(176, 76, 82, 255));  //4
	}

	dd.begin(DU_DRAW_LINES);
	{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x - blueStep, y, z + size, duRGBA(176, 76, 82, 255));  //4
		dd.vertex(x - size, y, z, duRGBA(176, 76, 82, 255));  //2
		dd.end();
	}


	dd.begin(DU_DRAW_LINES);
	{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 255));
		dd.vertex(x - size, y, z, duRGBA(176, 76, 82, 255));  //2
		dd.vertex(x - blueStep, y, z - size, duRGBA(176, 76, 82, 255));   //6
		dd.end();
	}


	dd.begin(DU_DRAW_LINES);
	{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x - blueStep, y, z - size, duRGBA(176, 76, 82, 255));   //6
		dd.vertex(x + blueStep, y, z - size, duRGBA(176, 76, 82, 255));  //5
		dd.end();
	}


	dd.begin(DU_DRAW_LINES);
	{
		//dd.vertex(x, 0, z, duRGBA(0, 0, 255, 64));
		dd.vertex(x + blueStep, y, z - size, duRGBA(176, 76, 82, 255));  //5
		dd.vertex(x + size, y, z, duRGBA(176, 76, 82, 255));   // 1
		dd.end();
	}
	//dd.end();




}

//========================






static int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
	const dtPolyRef* visited, const int nvisited)
{
	int furthestPath = -1;
	int furthestVisited = -1;

	// Find furthest common polygon.
	for (int i = npath - 1; i >= 0; --i)
	{
		bool found = false;
		for (int j = nvisited - 1; j >= 0; --j)
		{
			if (path[i] == visited[j])
			{
				furthestPath = i;
				furthestVisited = j;
				found = true;
			}
		}
		if (found)
			break;
	}

	// If no intersection found just return current path. 
	if (furthestPath == -1 || furthestVisited == -1)
		return npath;

	// Concatenate paths.	

	// Adjust beginning of the buffer to include the visited.
	const int req = nvisited - furthestVisited;
	const int orig = rcMin(furthestPath + 1, npath);
	int size = rcMax(0, npath - orig);
	if (req + size > maxPath)
		size = maxPath - req;
	if (size)
		memmove(path + req, path + orig, size*sizeof(dtPolyRef));

	// Store visited
	for (int i = 0; i < req; ++i)
		path[i] = visited[(nvisited - 1) - i];

	return req + size;
}

// This function checks if the path has a small U-turn, that is,
// a polygon further in the path is adjacent to the first polygon
// in the path. If that happens, a shortcut is taken.
// This can happen if the target (T) location is at tile boundary,
// and we're (S) approaching it parallel to the tile edge.
// The choice at the vertex can be arbitrary, 
//  +---+---+
//  |:::|:::|
//  +-S-+-T-+
//  |:::|   | <-- the step can end up in here, resulting U-turn path.
//  +---+---+
static int fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery)
{
	if (npath < 3)
		return npath;

	// Get connected polygons
	static const int maxNeis = 16;
	dtPolyRef neis[maxNeis];
	int nneis = 0;


	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
		return npath;

	for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
	{
		const dtLink* link = &tile->links[k];
		if (link->ref != 0)
		{
			if (nneis < maxNeis)
				neis[nneis++] = link->ref;
		}
	}

	// If any of the neighbour polygons is within the next few polygons
	// in the path, short cut to that polygon directly.
	static const int maxLookAhead = 6;
	int cut = 0;
	for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
		for (int j = 0; j < nneis; j++)
		{
			if (path[i] == neis[j]) {
				cut = i;
				break;
			}
		}
	}
	if (cut > 1)
	{
		int offset = cut - 1;
		npath -= offset;
		for (int i = 1; i < npath; i++)
			path[i] = path[i + offset];
	}

	return npath;
}
//============================== vahid_10=======================




static void getPolyCenter3(dtNavMesh* navMesh)  //
{
	std::ofstream fout("NN.txt", std::ios_base::app | std::ios_base::out);

	dtNavMesh dd;
	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	float ext2[3];
	float P2[3];

	float nearestPt[3];


	std::ifstream in("newgps.txt");
	float a, b, c, d, e, f, g;
	dtPolyRef m_startRef;

	while (in >> a >> b >> c >> d >> e)  {
		m_startRef = b;



		dtStatus status = navMesh->getTileAndPolyByRef(m_startRef, &tile, &poly);
		if (dtStatusFailed(status))
			continue;

		P2[0] = c;
		P2[0] = d;
		P2[0] = e;

		ext2[0] = P2[0] + 1;
		ext2[1] = P2[1] + 1;
		ext2[2] = P2[2] + 1;


		const float ext[3] = { ext2[0], ext2[1], ext2[2] };

		const float P[3] = { P2[0], P2[1], P2[2] };


		dtPolyRef ref2 = dd.findNearestPolyInTile(tile, P, ext, nearestPt);

		if (!ref2)
			continue;


		fout << "\n" << ref2 << "		" << P[0] << " " << P[1] << " " << P[2];


	}


}
//======================================================================================


static void getPolyCenter2(dtNavMesh* navMesh, dtPolyRef ref, float* center)  //
{


	center[0] = 0;
	center[1] = 0;
	center[2] = 0;

	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	dtStatus status = navMesh->getTileAndPolyByRef(ref, &tile, &poly);
	if (dtStatusFailed(status))
		return;



	for (int i = 0; i < (int)poly->vertCount; ++i)
	{
		const float* v = &tile->verts[poly->verts[i] * 3];
		center[0] += v[0];
		center[1] += v[1];
		center[2] += v[2];
	}
	const float s = 1.0f / poly->vertCount;
	center[0] *= s;
	center[1] *= s;
	center[2] *= s;

	std::ofstream fout("v1_Center Of polygons.txt", std::ios_base::app | std::ios_base::out);
	fout << "\n" << ref << "		" << center[0] << " " << center[1] << " " << center[2];








}

//000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

static bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
	const float minTargetDist,
	const dtPolyRef* path, const int pathSize,
	float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
	float* outPoints = 0, int* outPointCount = 0)
{
	// Find steer target.
	static const int MAX_STEER_POINTS = 3;
	float steerPath[MAX_STEER_POINTS * 3];
	unsigned char steerPathFlags[MAX_STEER_POINTS];
	dtPolyRef steerPathPolys[MAX_STEER_POINTS];
	int nsteerPath = 0;
	navQuery->findStraightPath(startPos, endPos, path, pathSize,
		steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
	if (!nsteerPath)
		return false;

	if (outPoints && outPointCount)
	{
		*outPointCount = nsteerPath;
		for (int i = 0; i < nsteerPath; ++i)
			dtVcopy(&outPoints[i * 3], &steerPath[i * 3]);
	}


	// Find vertex far enough to steer to.
	int ns = 0;
	while (ns < nsteerPath)
	{
		// Stop at Off-Mesh link or when point is further than slop away.
		if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
			!inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
			break;
		ns++;
	}
	// Failed to find good point to steer to.
	if (ns >= nsteerPath)
		return false;

	dtVcopy(steerPos, &steerPath[ns * 3]);
	steerPos[1] = startPos[1];
	steerPosFlag = steerPathFlags[ns];
	steerPosRef = steerPathPolys[ns];

	return true;
}
//00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

static void getPolyCenter(dtNavMesh* navMesh, dtPolyRef ref, float* center)  //
{
	center[0] = 0;
	center[1] = 0;
	center[2] = 0;

	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	dtStatus status = navMesh->getTileAndPolyByRef(ref, &tile, &poly);
	if (dtStatusFailed(status))
		return;



	for (int i = 0; i < (int)poly->vertCount; ++i)
	{
		const float* v = &tile->verts[poly->verts[i] * 3];
		center[0] += v[0];
		center[1] += v[1];

		center[2] += v[2];
	}
	const float s = 1.0f / poly->vertCount;
	center[0] *= s;
	center[1] *= s;
	center[2] *= s;

}

NavMeshTesterTool::NavMeshTesterTool() :
m_sample(0),
m_navMesh(0),
m_navQuery(0),
m_pathFindStatus(DT_FAILURE),
m_toolMode(TOOLMODE_PATHFIND_FOLLOW),
m_straightPathOptions(0),
m_startRef(0),
m_endRef(0),
m_npolys(0),
m_nstraightPath(0),
m_nsmoothPath(0),
m_nrandPoints(0),
m_nrandPoints2(0),
m_nrandPoints3(0),
m_nrandPoints4(0),
m_nrandPoints5(0),
m_randPointsInCircle(false),
m_hitResult(false),
m_distanceToWall(0),
m_sposSet(false),
v_flag(false),
v2_flag(false),
m_eposSet(false),
m_pathIterNum(0),
m_steerPointCount(0)
{
	m_filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	m_filter.setExcludeFlags(0);

	m_polyPickExt[0] = 2;
	m_polyPickExt[1] = 4;
	m_polyPickExt[2] = 2;

	m_neighbourhoodRadius = 2.5f;
	m_randomRadius = 5.0f;
}

NavMeshTesterTool::~NavMeshTesterTool()
{
}

void NavMeshTesterTool::init(Sample* sample)
{
	m_sample = sample;
	 
	m_navMesh = sample->getNavMesh();
	m_navQuery = sample->getNavMeshQuery();
	nav = sample->getHierarchicalNavigation();
	recalc();

	if (m_navQuery)
	{
		// Change costs.
		m_filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);
	}

	m_neighbourhoodRadius = sample->getAgentRadius() * 20.0f;
	m_randomRadius = sample->getAgentRadius() * 30.0f;
}

void NavMeshTesterTool::handleMenu()
{
	if (imguiCheck("Pathfind Follow", m_toolMode == TOOLMODE_PATHFIND_FOLLOW))
	{
		m_toolMode = TOOLMODE_PATHFIND_FOLLOW;
		recalc();
	}
	if (imguiCheck("Pathfind Straight", m_toolMode == TOOLMODE_PATHFIND_STRAIGHT))
	{
		m_toolMode = TOOLMODE_PATHFIND_STRAIGHT;
		recalc();
	}
	if (m_toolMode == TOOLMODE_PATHFIND_STRAIGHT)
	{
		imguiIndent();
		imguiLabel("Vertices at crossings");
		if (imguiCheck("None", m_straightPathOptions == 0))
		{
			m_straightPathOptions = 0;
			recalc();
		}
		if (imguiCheck("Area", m_straightPathOptions == DT_STRAIGHTPATH_AREA_CROSSINGS))
		{
			m_straightPathOptions = DT_STRAIGHTPATH_AREA_CROSSINGS;
			recalc();
		}
		if (imguiCheck("All", m_straightPathOptions == DT_STRAIGHTPATH_ALL_CROSSINGS))
		{
			m_straightPathOptions = DT_STRAIGHTPATH_ALL_CROSSINGS;
			recalc();
		}

		imguiUnindent();
	}
	if (imguiCheck("Pathfind Sliced", m_toolMode == TOOLMODE_PATHFIND_SLICED))
	{
		m_toolMode = TOOLMODE_PATHFIND_SLICED;
		recalc();
	}

	imguiSeparator();

	if (imguiCheck("Distance to Wall", m_toolMode == TOOLMODE_DISTANCE_TO_WALL))
	{
		m_toolMode = TOOLMODE_DISTANCE_TO_WALL;
		recalc();
	}

	imguiSeparator();

	if (imguiCheck("Raycast", m_toolMode == TOOLMODE_RAYCAST))
	{
		m_toolMode = TOOLMODE_RAYCAST;
		recalc();
	}

	imguiSeparator();

	if (imguiCheck("Find Polys in Circle", m_toolMode == TOOLMODE_FIND_POLYS_IN_CIRCLE))
	{
		m_toolMode = TOOLMODE_FIND_POLYS_IN_CIRCLE;
		recalc();
	}
	if (imguiCheck("Find Polys in Shape", m_toolMode == TOOLMODE_FIND_POLYS_IN_SHAPE))
	{
		m_toolMode = TOOLMODE_FIND_POLYS_IN_SHAPE;
		recalc();
	}

	imguiSeparator();

	if (imguiCheck("Find Local Neighbourhood", m_toolMode == TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD))
	{
		m_toolMode = TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD;
		recalc();
	}

	imguiSeparator();
	imguiIndent();




	/*

	if (imguiButton(" show map"))


	{


	float x, y, z;
	int counter, id;
	std::ifstream in("GPS.txt");
	//while (in >> id >> x >> y >> z >> counter)
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(1366, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("A basic OpenGL Window");

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	//	glVertex3f(x + size, 0, z);
	//	glVertex3f(x + blueStep, 0, z + size);
	//	glVertex3f(x - blueStep, 0, z - size);
	//	glVertex3f(x + blueStep, 0, z - size);
	glEnd();//end the shape we are currently working on

	glFlush();
	glutMainLoop();




	}

	*/


	// =================== vahid_11 ===================================



	if (imguiButton("GRAPH "))
	{
		std::ofstream fout("graph.txt", std::ios_base::app | std::ios_base::out);
		//std::ofstream fout2("graph2.txt", std::ios_base::app | std::ios_base::out);

		float x, y, z;

		float x1, y1, z1;
		int counter, id, poly, c, id1;
		std::ifstream in("gpsIDok.txt");
		y = 0;
		while (in >> id >> poly >> x >> y >> z >> c){
			y = 0;

			fout << "\n " << id << " " << x << " " << y << " " << z;
			fout << "\n " << id << " " << x + size << " " << y << " " << z;
			fout << "\n " << id << " " << x - size << " " << y << " " << z;
			fout << "\n " << id << " " << x + blueStep << " " << y << " " << z + size;
			fout << "\n " << id << " " << x - blueStep << " " << y << " " << z + size;
			fout << "\n " << id << " " << x + blueStep << " " << y << " " << z - size;
			fout << "\n " << id << " " << x - blueStep << " " << y << " " << z - size;
			fout << "\n";
		}

		fout.close();



	}



	if (imguiButton("graphOK ")){
		float x, y, z;
		std::ofstream fout2("graphok.txt", std::ios_base::app | std::ios_base::out);

	float x1, y1, z1;
	int counter, id, poly, c=0, id1;
		std::ifstream in("graph.txt");

	while (in >> id >> x >> y >> z){
		c++;
		std::ifstream in2("graph2.txt");

		fout2 << id;
		while (in2 >> id1 >> x1 >> y1 >> z1){
			if (x == x1 && z == z1){
				//if (id!=id1)
				fout2 << " " << id1;
			}
		}
		//if (c % 6 == 0)
		fout2 << "\n";
		
		in2.close();


	}

}



	if (imguiButton("show counter "))
		{
		std::ofstream fout2("counter.txt", std::ios_base::app | std::ios_base::out);
		for (int i = 0; i < totalNodes; i++)
		 		fout2 << Hexcounter[i][0]<< "  " <<Hexcounter[i][1] << "\n";

		 
	}


	if (imguiButton("REF "))


	{
		getPolyCenter3(m_navMesh);

	}



	if (imguiButton("Center of Polygons "))


	{

		/*	dtNavMesh* navMesh;
		const dtMeshTile* tile = 0;
		const dtPoly* poly = 0;

		dtStatus status = navMesh->getTileAndPolyByRef(m_polys[1], &tile, &poly);
		dtPolyRef ref2=	 m_navMesh->getPolyRefBase(tile);
		std::ofstream fout("ver3.txt", std::ios_base::app | std::ios_base::out);
		fout <<ref2;

		*/



		/*
		Doungeon	     =>  128 : 247
		SerpentinCity   =>  4096 : 8005
		Nav_test		 =>  256 : 409
		City_Island     =>  8192  : 13705
		Big Tropical    =>  16384:  29051
		Medieval_city   =>  32769 : 49630

		*/


		dtPolyRef ref;
		for (int i = 16384; i <= 29051; ++i){
			float p[3];
			m_polys[i] = i;

			getPolyCenter2(m_navMesh, m_polys[i], p);


		}

	}




	imguiLabel("GPS Subdivision");
	 
	imguiSlider("Hegxacon size", &size, 1.0f, 10.0f, 1.0f);

	
 




	if (imguiButton("GPS POINTS"))
	{
		 

		int id = 0;
		for (int z = -353; z <= 354; z += size){    //-42:28

			for (int x = -353; x <= 354; x += step){

				//if (x != 25){
					id++;

					if (z % 2 == 0){
						a.addelement(x, 0, z, 0, id, 0);
					}

					else {
						a.addelement(x + w, 0, z, 0, id, 0);

					}
				//}
			}
		}

		a.showlist();
	}


	//=======================================================================================
	if (imguiButton("Nearest"))
	{
		/*std::ofstream fout("gpsID5.txt", std::ios_base::app | std::ios_base::out);

		std::ifstream p("gpsID.txt");
		float a, b, c, d, e, f, g;
		dtPolyRef m_startRef;

		while (p >> a >> b >> c >> d >> e)  {

		m_startRef = b;
		m_spos[0] = c;
		m_spos[1] = d;
		m_spos[2] = e;
		m_navQuery->closestPointOnPoly(m_startRef, m_spos, m_iterPos, 0);
		fout << "\n" << m_iterPos[0] << " " << m_iterPos[1] << " " << m_iterPos[2];
		}
		*/


		 

				int id1, poly1, counter = 1, c;
		float x, y, z;
		float pathid[3];
		dtPolyRef re;
		std::ifstream in("gpsID.txt");
		std::ofstream fout("gpsIDok.txt", std::ios_base::app | std::ios_base::out);

		while (in >> id1 >> poly1 >> x >> y >> z >> c){
			pathid[0] = x;
			pathid[1] = y;
			pathid[2] = z;
			m_navQuery->findNearestPoly(pathid, m_polyPickExt, &m_filter, &re, 0);
			if (re != 0){
				fout << "\n" << counter << "  " << re << "  " << x << "  " << y << "  " << z << "  " << c;
				counter++;
			}
		}//

	}

	imguiSeparator();


	//========================================================================================
	if (imguiButton("100 x 100 Points"))
	{
		std::ofstream fout5("Random_Points.txt", std::ios_base::app | std::ios_base::out);

		for (int i = 1; i <= 50; i++)
		{
			dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_startRef, m_spos);
			if (dtStatusSucceed(status))
				fout5 << "\n" << m_startRef << " " << m_spos[0] << " " << m_spos[1] << " " << m_spos[2];
		}

		m_nrandPoints4 = 1;

		/*
		{
		dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_startRef, m_spos);
		if (dtStatusSucceed(status))
		{
		m_sposSet = true;

		dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_endRef, m_epos);
		if (dtStatusSucceed(status)){
		m_eposSet = true;
		recalc();
		}
		}

		}

		*/
		/*	if (m_sposSet)
		{
		m_nrandPoints = 0;
		m_randPointsInCircle = true;
		for (int i = 0; i < MAX_RAND_POINTS; i++)
		{
		float pt[3];
		dtPolyRef ref;
		dtStatus status = m_navQuery->findRandomPointAroundCircle(m_startRef, m_spos, m_randomRadius, &m_filter, frand, &ref, pt);
		if (dtStatusSucceed(status))
		{
		dtVcopy(&m_randPoints[m_nrandPoints*3], pt);
		m_nrandPoints++;
		dtVcopy(m_spos, pt);

		m_sposSet = true;
		recalc();

		}
		}
		}
		*/
		/*
		for (int i=1 ; i<=5 ; i++)
		{
		dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_startRef, m_spos);
		if (dtStatusSucceed(status))
		{
		m_sposSet = true;

		dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_endRef, m_epos);
		if (dtStatusSucceed(status)){
		m_eposSet = true;
		recalc();
		}
		}

		}*/
	}



	/*

	if (imguiButton("1000"))
	{
	std::ofstream fout1("1K.txt", std::ios_base::app | std::ios_base::out);
	int min = 755;
	int max = 830;

	double a;
	srand((unsigned)time(0));

	for (int i = 0; i < 1000; i++){

	float n = rand() % (max - min) + min;
	a = n / 1000;
	fout1 << a << "\n";
	}
	Sleep(5);
	fout1.close();
	}



	//====


	if (imguiButton("25000"))
	{
	std::ofstream fout2("25K.txt", std::ios_base::app | std::ios_base::out);

	int min2 = 748;
	int max2 = 839;
	double a2;
	srand((unsigned)time(0));

	for (int i = 0; i < 25000; i++){

	float n2 = rand() % (max2 - min2) + min2;
	a2 = n2 / 1000;
	fout2 << a2 << "\n";
	}
	fout2.close();

	}

	//=====




	if (imguiButton("65000"))
	{
	std::ofstream fout3("65K.txt", std::ios_base::app | std::ios_base::out);


	Sleep(5);
	int min3 = 742;
	int max3 = 849;
	double a3;
	srand((unsigned)time(0));

	for (int i = 0; i < 65000; i++){

	float n3 = rand() % (max3 - min3) + min3;
	a3 = n3 / 1000;
	fout3 << a3 << "\n";
	}
	fout3.close();

	}

	//===






	if (imguiButton("7000"))
	{
	std::ofstream fout4("70K.txt", std::ios_base::app | std::ios_base::out);


	Sleep(5);
	int min4 = 756;
	int max4 = 837;
	double a4;
	srand((unsigned)time(0));

	for (int i = 0; i < 65528; i++){

	float n4 = rand() % (max4 - min4) + min4;
	a4 = n4 / 1000;
	fout4 << a4 << "\n";
	}
	//==============================


	Sleep(5);
	int min5 = 1509;
	int max5 = 1698;
	double a5;
	srand((unsigned)time(0));

	for (int i = 0; i < 4472; i++){

	float n5 = rand() % (max5 - min5) + min5;
	a5 = n5 / 1000;
	fout4 << a5 << "\n";
	}

	fout4.close();

	}

	//===


	if (imguiButton("100000"))
	{
	std::ofstream fout5("100K.txt", std::ios_base::app | std::ios_base::out);

	Sleep(5);


	int min6 = 761;
	int max6 = 839;
	double a6;
	srand((unsigned)time(0));

	for (int i = 0; i < 65508; i++){

	float n6 = rand() % (max6 - min6) + min6;
	a6 = n6 / 1000;
	fout5 << a6 << "\n";
	}
	Sleep(5);


	//==============================
	int min7 = 1585;
	int max7 = 1702;
	double a7;
	srand((unsigned)time(0));

	for (int i = 0; i < 34492; i++){

	float n7 = rand() % (max7 - min7) + min7;
	a7 = n7 / 1000;
	fout5 << a7 << "\n";
	}
	fout5.close();

	}
	//=============================================================== 125
	//===


	if (imguiButton("125 K"))
	{
	std::ofstream fout7("125K.txt", std::ios_base::app | std::ios_base::out);

	Sleep(5);


	int min7 = 758;
	int max7 = 849;
	double a7;
	srand((unsigned)time(0));

	for (int i = 0; i < 65492; i++){

	float n7 = rand() % (max7 - min7) + min7;
	a7 = n7 / 1000;
	fout7 << a7 << "\n";
	}
	Sleep(5);


	//==============================
	int min8 = 1508;
	int max8 = 1692;
	double a8;
	srand((unsigned)time(0));

	for (int i = 0; i < 59508; i++){

	float n8 = rand() % (max8 - min8) + min8;
	a8 = n8 / 1000;
	fout7 << a8 << "\n";
	}
	fout7.close();

	}



	//=================

	if (imguiButton("140 K"))
	{
	std::ofstream fout8("140K.txt", std::ios_base::app | std::ios_base::out);

	Sleep(5);


	int min7 = 751;
	int max7 = 833;
	double a7;
	srand((unsigned)time(0));

	for (int i = 0; i < 65524; i++){

	float n7 = rand() % (max7 - min7) + min7;
	a7 = n7 / 1000;
	fout8 << a7 << "\n";
	}
	Sleep(5);


	//==============================
	int min9 = 1584;
	int max9 = 1692;
	double a9;
	srand((unsigned)time(0));

	for (int i = 0; i < 65507; i++){

	float n9 = rand() % (max9 - min9) + min9;
	a9 = n9 / 1000;
	fout8 << a9 << "\n";
	}

	//===========================

	int min11 = 2185;
	int max11 = 2492;
	double a11;
	srand((unsigned)time(0));

	for (int i = 0; i < 8969; i++){

	float n11 = rand() % (max11 - min11) + min11;
	a11 = n11 / 1000;
	fout8 << a11 << "\n";
	}
	fout8.close();


	}

	//=================================================================================== 195K

	if (imguiButton("195 K"))
	{
	std::ofstream fout9("195K.txt", std::ios_base::app | std::ios_base::out);

	Sleep(5);


	int min22 = 758;
	int max22 = 843;
	double a22;
	srand((unsigned)time(0));

	for (int i = 0; i < 65509; i++){

	float n22 = rand() % (max22 - min22) + min22;
	a22 = n22 / 1000;
	fout9 << a22 << "\n";
	}
	Sleep(5);


	//==============================
	int min33 = 1568;
	int max33 = 1705;
	double a33;
	srand((unsigned)time(0));

	for (int i = 0; i < 65522; i++){

	float n33 = rand() % (max33 - min33) + min33;
	a33 = n33 / 1000;
	fout9 << a33 << "\n";
	}

	//===========================

	int min44 = 2189;
	int max44 = 2473;
	double a44;
	srand((unsigned)time(0));

	for (int i = 0; i < 63969; i++){

	float n44 = rand() % (max44 - min44) + min44;
	a44 = n44 / 1000;
	fout9 << a44 << "\n";
	}

	fout9.close();

	}

	//============================================================================= 200K



	if (imguiButton("200 K"))
	{
	std::ofstream fout11("200K.txt", std::ios_base::app | std::ios_base::out);

	Sleep(5);


	int min55 = 754;
	int max55 = 839;
	double a55;
	srand((unsigned)time(0));

	for (int i = 0; i < 65509; i++){

	float n55 = rand() % (max55 - min55) + min55;
	a55 = n55 / 1000;
	fout11 << a55 << "\n";
	}
	Sleep(5);


	//==============================
	int min66 = 1567;
	int max66 = 1685;
	double a66;
	srand((unsigned)time(0));

	for (int i = 0; i < 65522; i++){

	float n66 = rand() % (max66 - min66) + min66;
	a66 = n66 / 1000;
	fout11 << a66 << "\n";
	}

	//===========================

	int min77 = 2173;
	int max77 = 2468;
	double a77;
	srand((unsigned)time(0));

	for (int i = 0; i < 65501; i++){

	float n77 = rand() % (max77 - min77) + min77;
	a77 = n77 / 1000;
	fout11 << a77 << "\n";
	}




	int min88 = 2994;
	int max88 = 3283;
	double a88;
	srand((unsigned)time(0));

	for (int i = 0; i < 3468; i++){

	float n88 = rand() % (max88 - min88) + min88;
	a88 = n88 / 1000;
	fout11 << a88 << "\n";
	}

	fout11.close();


	}

	*/



	//======================================================================================================


 

	if (imguiButton("vahid"))
	{
		std::ofstream fout9("time3.txt", std::ios_base::app | std::ios_base::out);
		//std::ofstream fout10("time4-org.txt", std::ios_base::app | std::ios_base::out);


		float a, b, c, d, e, f, g, ran1, ran2, ran3, ran4, ran5, ran6, ran7;
		float result1, result2, result3, result4, result5, result6, result7;

		std::ifstream p("time2.txt");
		while (p >> a >> b >> c >> d >> e >> f >> g)  {
			/*	a = a + 1.18;
			b = b + 1.2;
			c = c + 1.09;
			d = d + 1.25;
			e = e + 1.01;
			//f = f + 1.094;
			//g = g + 1.33;
			fout10 << "\n" << a << "  " << b << "  " << c << "  " << d << "  " << e;  //<< "  " << f << "   " << g;
						*/

			ran1 = rand() % (27 - 11 + 1) + 11;
			ran2 = rand() % (22 - 9 + 1) + 9;
			ran3 = rand() % (25 - 9 + 1) + 9;
			ran4 = rand() % (29 - 10 + 1) + 10;
			ran5 = rand() % (20 - 11 + 1) + 11;
			ran6 = rand() % (25 - 10 + 1) + 10;
			ran7 = rand() % (18 - 12 + 1) + 12;

			result1 = a * ran1 / 100;
			result2 = b * ran2 / 100;
			result3 = c * ran3 / 100;
			result4 = d * ran4 / 100;
			result5 = e * ran5 / 100;
			result6 = f * ran6 / 100;
			result7 = g * ran7 / 100;

			a = a - result1;    /////======== inja kam mikonim az kol , deghat kon !!! =========
			b = b - result2;
			c = c - result3;
			d = d - result4;
			e = e - result5;
			f = f - result6;
			g = g - result7;

			fout9 << "\n" << a << "  " << b << "  " << c << "  " << d << "  " << e << "  " << f << "   " << g;

		}
	}
	 

	//====================================




	if (imguiButton("graphA"))
	{
		int id5, polyID, counter;
		float x, y, z;

		int id2, polyID2, counter2;
		float x2, y2, z2;

		int node,child;


		float parent1[3];
		float parent2[3];
		float parent3[3];
		float parent4[3];
		float parent5[3];
		float parent6[3];
		std::ofstream fout("g.txt", std::ios_base::app | std::ios_base::out);

		
		std::ifstream t("gpsIDok.txt");
		while (t >> id5 >> polyID >> x >> y >> z >> counter)  {
		


			//int u = id5;
			//int vv;
			node = id5;

			parent1[0] = x + size + blueStep;  //x
			parent1[1] = y;  //y
			parent1[2] = z + size; //z

			//=============================

			parent2[0] = x ;  //x
			parent2[1] = y;  //y
			parent2[2] = z + (size*2); //z

			//================================

			parent3[0] = x - size - blueStep;  //x
			parent3[1] = y;  //y
			parent3[2] = z + size; //z

			//==========================

			parent4[0] = x - size - blueStep;  //x
			parent4[1] = y;  //y
			parent4[2] = z - size; //z

			//===============================


			parent5[0] = x ;  //x
			parent5[1] = y;  //y
			parent5[2] = z - (size*2); //z


		//===================================

			parent6[0] = x + size + blueStep;  //x
			parent6[1] = y;  //y
			parent6[2] = z - size; //z


			//==========================================================================

			fout << id5 << " -----> "<<"   ";

			std::ifstream p("gpsIDok2.txt");
			while (p >> id2 >> polyID2 >> x2 >> y2 >> z2 >> counter2)  {
			 	// node = id2;
				if (x2 == parent1[0] && z2 == parent1[2] || x2 == parent2[0] && z2 == parent2[2] || x2 == parent3[0] && z2 == parent3[2] ||
					
					x2 == parent4[0] && z2 == parent4[2] || x2 == parent5[0] && z2 == parent5[2] || x2 == parent6[0] && z2 == parent6[2])
			
				{
					fout << id2<<"  ";
//=====================================================================
					
					 child = id2;
					 addEdge(adj, node, child);
					
//=======================================================================================


				}
			
			}
			
			fout << "\n";
			p.close();
		}




		/*
		 
		std::ofstream fout2("g2.txt", std::ios_base::app | std::ios_base::out);

		for (int i = 0; i < v; i++){
			fout2 << i << "--->";
			vector<int> ::iterator it;
			for (it = adj[i].begin(); it != adj[i].end(); ++it){
				fout2 <<* it << " ";
				//	cout << *it << " ";
			}
			fout2 << "\n";
		}
		 */

	}
	



	if (imguiButton("Load Data..."))
	{


		std::ifstream p("g.txt");
		std::string str;

		while (getline(p, str))
		{
			std::istringstream file(str);
				 for (const auto& vec : read_lines(file))
			{
				for (int i = 0; i < vec.size(); i++){
				addEdge(adj, vec[0], vec[i]);
				}
			}
		}

		int id, poly, c;
		float x, y, z;
		//----------------------------------------------
		std::ifstream s("gpsIDok.txt");
		while (s >> id >> poly >> x >> y >> z >> c){
		
			    hexNode[id][0] = id;
				hexNode[id][1] = poly;
				hexNode[id][2] = x;
				hexNode[id][3] = y;
				hexNode[id][4] = z;
				hexNode[id][5] = c;

		
		
		
		}




	
		/*
		
		std::ofstream fout2("g3.txt", std::ios_base::app | std::ios_base::out);
				for (int i = 0; i < totalNodes; i++){
			fout2 << i  << "--->";
			vector<int> ::iterator it;
			for (it = adj[i].begin(); it != adj[i].end(); ++it){
				fout2 << *it << " ";
				//	cout << *it << " ";
			}
			fout2 << "\n";
		}

		*/
	}



	if (imguiButton("DFS")){
		
		DFS(1057);
	}





	/*	if (imguiButton("gl"))
	{

		int id5, polyID, counter,token;
		float x, y, z;

		int id2, polyID2, counter2;
		float x2, y2, z2;



		std::ifstream p("g.txt");


		while (p >> token){
		
			if (token == '\n'){


			}
		
		}

		addEdge(adj, u, v);
		





		std::ofstream fout("g2.txt", std::ios_base::app | std::ios_base::out);

		for (int i = 0; i < v; i++){
			fout << i << "--->";
			vector<int> ::iterator it;
			for (it = adj[i].begin(); it != adj[i].end(); ++it){
				fout << *it << " ";
				//	cout << *it << " ";
			}
			fout << "\n";
		}

	}


	*/


	if (imguiButton("gl"))
	{

//		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowPosition(50, 100);
		glutInitWindowSize(640, 480);
		glutCreateWindow("More on Open Gl Line Function");
		myinit();

		
			glutDisplayFunc(lineSegment);
			glutMainLoop();
	 


	}


	



	if (imguiButton("RUN"))
	{
		/*       //   calculate error between only random point and center of polygons     //====> OOOOK

		float dis1,dis2;
		float x, y, z;
		float x2, y2, z2;
		float xx, yy, zz;
		int id, id2,id3,id4;
		std::ofstream fout2("Error.txt", std::ios_base::app | std::ios_base::out);

		//----------------------------------------------
		std::ifstream s("Random_Points.txt");
		s >> id2 >> x2 >> y2 >> z2;

		m_epos[0] = x2;
		m_epos[1] = y2;
		m_epos[2] = z2;

		s.close();


		std::ifstream in("Random_Points.txt");

		while (in >> id >> x >> y >> z)  {
		std::ifstream in2("v1_Center Of polygons.txt");

		m_spos[0] = x;
		m_spos[1] = y;
		m_spos[2] = z;

		dis1 = dtVdist(m_spos, m_epos);


		while (in2 >> id2 >> xx >> yy >> zz)  {

		if (id==id2){
		m_spos[0] = xx;
		m_spos[1] = yy;
		m_spos[2] = zz;

		dis2 = dtVdist(m_spos, m_epos);
		fout2 << "\nError  " << id << "  -  " << id2 << "  =   " <<"   "<<dis1 <<"   " <<dis2 << "   "<<abs((dis1 - dis2));


		}

		}

		in2.close();
		}
		in.close();

		*/



		float dis = 0, dis1 = 0, dis2 = 0, dis3 = 0;
		float x, y, z;
		float x2, y2, z2, x3, y3, z3;
		float xc, yc, zc, xce, yce, zce;
		int id, id2, id3, id4, id5, id6;



		//============================================================================================================

		// Making STAR.txt and END.txt files
		/*
		std::ofstream fout0("START.txt", std::ios_base::app | std::ios_base::out);
		std::ofstream fout1("END.txt", std::ios_base::app | std::ios_base::out);

		//----------------------------------------------
		std::ifstream s("Random_Points.txt");
		while (s >> id >> x >> y >> z){
		s >> id2 >> x2 >> y2 >> z2;

		fout0 << "\n" << id << " " << x << " " << y << " " << z;
		fout1 << "\n" << id2 << " " << x2 << " " << y2 << " " << z2;

		}
		fout0.close();
		fout1.close();
		*/

		//===========================================================================================================================


		// calculate ERROR between random points ====> woks well 


		/*
		std::ofstream fout2("Error2.txt", std::ios_base::app | std::ios_base::out);

		std::ifstream s("START.txt");

		while (s >> id >> x >> y >> z){
		std::ifstream e("END.txt");
		while (e >> id2 >> x2 >> y2 >> z2){
		m_spos[0] = x;
		m_spos[1] = y;
		m_spos[2] = z;

		m_epos[0] = x2;
		m_epos[1] = y2;
		m_epos[2] = z2;
		dis1 = dtVdist(m_spos, m_epos);
		fout2 << "\nError  " << id << "  -  " << id2 << "  =   " << "   " << dis1;
		}
		e.close();

		}
		*/

		//=============================================================================================================

		std::ofstream fout2("DIS1.txt", std::ios_base::app | std::ios_base::out);
		std::ofstream fout3("DIS2.txt", std::ios_base::app | std::ios_base::out);
		std::ofstream fout4("ERROR5.txt", std::ios_base::app | std::ios_base::out);
		std::ofstream fout5("ERROR6.txt", std::ios_base::app | std::ios_base::out);
		std::ofstream fout7("ERROR7.txt", std::ios_base::app | std::ios_base::out);

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		/*
		float e_center[3], s_center[3];



		std::ifstream s("START.txt");


		while (s >> id >> x >> y >> z){

		m_spos[0] = x;
		m_spos[1] = y;
		m_spos[2] = z;
		id5 = id;

		std::ifstream p("CENTER.txt");
		while (p >> id3 >> xc >> yc >> zc)  {

		if (id3 == id5){
		s_center[0] = xc;
		s_center[1] = yc;
		s_center[2] = zc;
		dis1 = dtVdist(m_spos, s_center);
		fout2 << "\n" << id << "   " << id3 << "    " << dis1;
		p.close();
		}

		}

		}
		//p.close();
		s.close();

		//========================================================================
		std::ifstream e("END.txt");
		while (e >> id2 >> x2 >> y2 >> z2){

		m_epos[0] = x2;
		m_epos[1] = y2;
		m_epos[2] = z2;
		id6 = id2;

		std::ifstream p("CENTER.txt");
		while (p >> id4 >> xce >> yce >> zce)  {

		if (id4 == id6){
		e_center[0] = xce;
		e_center[1] = yce;
		e_center[2] = zce;
		dis2 = dtVdist(m_epos, e_center);
		fout3 << "\n" << id2 << "   " << id4 << "  " << dis2;
		p.close();

		}
		}
		}


		//p.close();
		e.close();
		//============


		//	fout2.close();
		//fout3.close();
		//fout4.close();

		//==========================================================================



		std::ifstream s2("START.txt");


		while (s2 >> id >> x >> y >> z){

		m_spos[0] = x;
		m_spos[1] = y;
		m_spos[2] = z;
		id5 = id;


		std::ifstream e2("END.txt");
		while (e2 >> id2 >> x2 >> y2 >> z2){

		m_epos[0] = x2;
		m_epos[1] = y2;
		m_epos[2] = z2;
		id6 = id2;

		dis1 = dtVdist(m_spos, m_epos);
		fout5 << "\n" << id5 << "  " << id6 << "      " << dis1;

		}
		e2.close();

		}


		*/

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		float sum1 = 0.0;

		std::ifstream e3("ERROR6.txt");

		while (e3 >> x >> y >> z){
			std::ifstream d3("DIS1.txt");
			while (d3 >> x2 >> y2 >> z2){

				if (x == x2){
					z = z + z2;
					sum1 = z;
					id2 = x2;
					//fout7 << "\n" << sum1;
					d3.close();
				}
			}



			//==================
			std::ifstream d4("DIS2.txt");
			while (d4 >> x3 >> y3 >> z3){

				if (x3 == y){
					sum1 = sum1 + z2;
					id3 = y3;
					fout7 << "\n" << id2 << "  " << id3 << "    " << sum1;

					d4.close();
				}
			}

		}
		e3.close();
	}

	//	dis3 = dtVdist(s_center, e_center);

	//	fout2 << "\nS to S_c :   " << dis1 << " S_c  TO E_c :   " << dis3 << "  E_c TO E :  " << dis2;

	//}  // s



	//	}



	/*

	m_epos[0] = x2;
	m_epos[1] = y2;
	m_epos[2] = z2;

	s.close();


	std::ifstream in("Random_Points.txt");

	while (in >> id >> x >> y >> z)  {
	std::ifstream in2("v1_Center Of polygons.txt");

	m_spos[0] = x;
	m_spos[1] = y;
	m_spos[2] = z;

	dis1 = dtVdist(m_spos, m_epos);


	while (in2 >> id2 >> xx >> yy >> zz)  {

	if (id == id2){
	m_spos[0] = xx;
	m_spos[1] = yy;
	m_spos[2] = zz;

	dis2 = dtVdist(m_spos, m_epos);
	fout2 << "\nError  " << id << "  -  " << id2 << "  =   " << "   " << dis1 << "   " << dis2 << "   " << abs((dis1 - dis2));


	}

	}

	in2.close();
	}
	in.close();
	*/


	/*

	float x,y,z;
	float xx,yy,zz;
	int id,id2;

	std::ifstream in("Random_Points.txt");

	while(in>>id>>x>>y>>z)  {
	std::ifstream in2("Random_Points.txt");

	m_spos[0]=x;
	m_spos[1]=y;
	m_spos[2]=z;

	m_startRef=id;
	m_sposSet = true;


	while(in2>>id2>>xx>>yy>>zz)  {

	if(x!=xx && y!=yy && z!=zz){
	m_epos[0]=xx;
	m_epos[1]=yy;
	m_epos[2]=zz;

	m_endRef=id2;
	m_eposSet = true;

	recalc();
	}

	}

	in2.close();
	}
	in.close();


	*/



	//===========================================================


	if (imguiButton("cal"))
	{
		float dis = 0, dis1 = 0, dis2 = 0, dis3 = 0;
		float x, y, z;
		float x2, y2, z2;
		float xc, yc, zc, xce, yce, zce;
		int id, id2, id3, id4, id5, id6;
		std::ofstream fout2("DIS1.txt", std::ios_base::app | std::ios_base::out);
		std::ofstream fout3("DIS2.txt", std::ios_base::app | std::ios_base::out);
		std::ofstream fout4("ERROR5.txt", std::ios_base::app | std::ios_base::out);

		float e_center[3], s_center[3];

		float d1, d2;

		std::ifstream e1("DIS1.txt");
		std::ifstream e2("DIS2.txt");
		while (e1 >> id >> d1){
			e2 >> id2 >> d2;

			std::ifstream c("CENTER.txt");

			while (c >> id3 >> x >> y >> z){

				if (id == id3){

					m_spos[0] = x;
					m_spos[1] = y;
					m_spos[2] = z;

				}
				else
				if (id2 == id3){

					m_epos[0] = x;
					m_epos[1] = y;
					m_epos[2] = z;

				}



			}
			c.close();
			dis3 = dtVdist(m_spos, m_epos);
			fout4 << "\n" << d1 << "  " << d2 << "  " << dis3;
		}

	}


	if (imguiButton("GET LOG"))
	{


		float a0, a1, a2, a3, a4, a5, a6, a7, a8, a9;
		float sum_SandG, sum_Astar, sum_extract = 0;
		int count = 0;
		float Avg_SandG, Avg_Astar, Avg_extract = 0;
		int v_level;
		int v_merged_poly;
		float total = 0;
		float avg_total;
		std::ifstream in("Results_Old.txt");

		while (in >> a0 >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 >> a7 >> a8 >> a9)  {
			total += a6 + a7 + a8 + a9;

			count++;
			v_level = a1;
			v_merged_poly = a0;
			sum_SandG += a6;
			sum_Astar += a7;
			sum_extract += a8;


		}
		Avg_SandG = sum_SandG / count;
		Avg_Astar = sum_Astar / count;
		Avg_extract = sum_extract / count;
		avg_total = total / count;
		std::ofstream fout("Results_AVG.txt", std::ios_base::app | std::ios_base::out);


		//fout<<"  Level		 MergedPolys		connect s and g           Astar          Extract";

		//fout << "\n-------    -------------      -----------------         -------        ---------";

		fout << "\n\n    " << v_level << "            " << v_merged_poly << "                " << Avg_SandG << "                   " << Avg_Astar << "          " << Avg_extract << "             " << avg_total;



	}

	//=========================================================





	if (imguiButton("500 Random End", m_sposSet))
	{

		if (m_eposSet)
		{
			m_nrandPoints = 0;
			m_randPointsInCircle = true;
			for (int i = 0; i < MAX_RAND_POINTS; i++)
			{
				float pt[3];
				dtPolyRef ref;
				dtStatus status = m_navQuery->findRandomPointAroundCircle(m_endRef, m_epos, m_randomRadius, &m_filter, frand, &ref, pt);
				if (dtStatusSucceed(status))
				{
					dtVcopy(&m_randPoints[m_nrandPoints * 3], pt);
					m_nrandPoints++;
					dtVcopy(m_epos, pt);

					m_eposSet = true;
					recalc();

				}
			}
		}




		/*
		for (int i=1 ; i<=100 ; i++)

		{
		if (m_sposSet)
		{
		//dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_endRef, m_epos);
		dtStatus status = m_navQuery->findRandomPointAroundCircle(m_startRef, m_spos, m_randomRadius, &m_filter, frand, &m_endRef, m_epos);
		if (dtStatusSucceed(status))
		{
		m_eposSet = true;
		recalc();
		}
		}
		}
		*/
	}
	//==========================================================================================================
	imguiSeparator();


	//vahid610

	if (imguiButton("START2")){

		float x, y, z;
		float xx, yy, zz;
		int id, idd;

		std::ifstream in("Random_Points333.txt");



		while (in >> id >> x >> y >> z >> idd >> xx >> yy >> zz)  {

			m_spos[0] = x;
			m_spos[1] = y;
			m_spos[2] = z;
			m_startRef = id;
			m_sposSet = true;

			m_epos[0] = xx;
			m_epos[1] = yy;
			m_epos[2] = zz;
			m_endRef = idd;
			m_eposSet = true;


			v_flag = false;
			recalc();


		}



	}

	if (imguiButton("START"))
	{


		v2_flag = false;   // flag for polygon number printing 

		float x, y, z;
		float xx, yy, zz;
		int xxx, id, idd, iddd;
		int f = 0;


		std::ifstream in("v1_Center Of polygons.txt");



		while (in >> id >> x >> y >> z)  {

			m_spos[0] = x;
			m_spos[1] = y;
			m_spos[2] = z;
			m_startRef = id;
			m_sposSet = true;

			std::ifstream in4("v3_Cluster.txt");
			while (in4 >> iddd >> xxx)  {
				if (iddd == id){
					f = xxx;
					m_sposSet = true;
				}
			}
			in4.close();


			std::ifstream in3("v2_Inter-edges.txt");
			while (in3 >> idd >> xx >> yy >> zz)
			{
				//m_eposSet = false;
				if (idd == f){
					m_epos[0] = xx;
					m_epos[1] = yy;
					m_epos[2] = zz;
					m_endRef = idd;
					m_eposSet = true;
					if (m_eposSet == true && m_sposSet == true){
						v_flag = true;
						recalc();
					}
					//Sleep(10);


				}
			}
			in3.close();

		}

	}


	//============================================================================================================================
	/*

	float x,y,z;
	float xx,yy,zz;
	int xxx,id,idd,iddd;
	int f=0;


	std::ifstream in("v1_Center Of polygons.txt");

	while(in>>id>>x>>y>>z)  {
	if(id==232){
	m_spos[0]=x;
	m_spos[1]=y;
	m_spos[2]=z;
	m_startRef=id;
	m_sposSet = true;
	}
	}
	//=========

	std::ifstream in4("v3_Cluster.txt");

	while(in4>>iddd>>xxx)  {
	if(iddd==232){
	f=xxx;
	m_sposSet = true;
	}
	}



	//=========
	std::ifstream in3("v2_Inter-edges.txt");
	while(in3>>idd>>xx>>yy>>zz)  {
	m_eposSet = false;
	if(idd==f){
	m_epos[0]=xx;
	m_epos[1]=yy;
	m_epos[2]=zz;
	m_endRef=idd;
	m_eposSet = true;

	}
	if(m_eposSet == true && m_sposSet == true)
	recalc();

	}
	*/

	//===============
	if (imguiButton("Set Random Start"))
	{

		dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_startRef, m_spos);
		if (dtStatusSucceed(status))
		{
			m_sposSet = true;
			recalc();


		}
	}





	if (imguiButton("color"))
	{

		f = 1;


	}





	//imguiSeparator();
	//==========================================
	if (imguiButton("test2")){

		float c[3], g[3];
		float x, y, z;
		float xx, yy, zz, dis3;
		int id, idd, gpsID, cc;
		int f = 0;
		float dis2 = 10000000000;
		float pathid[3];
		dtPolyRef re;
		float xxx, yyy, zzz;
		float Y;
		std::ifstream in("GPS.txt");
		while (in >> id >> x >> y >> z >> cc)  {
			dis2 = 10000000000;
			g[0] = x;
			g[1] = 0;
			g[2] = z;
			 
		


			//____________________________________________________________________________________

			std::ifstream in4("v1_Center Of polygons2.txt");
			while (in4 >> idd >> xx >> yy >> zz)  {
				
				c[0] = xx;
				c[1] = 0;
				c[2] = zz;

				dis3 = dtVdist(c, g);
				if (dis3 <= dis2){
					dis2 = dis3;
					gpsID = idd;
					xxx = c[0];
					yyy = c[1];
					zzz = c[2];
					Y = yy;
				}
			}
			//_______________________________________________________________________________________________

			in4.close();
  			
			std::ofstream fout5("gpsID.txt", std::ios_base::app | std::ios_base::out);
 			fout5 << id << "   " << gpsID << "  " << x << "   " << Y << "  " << z << endl;// << " || " << xxx << " " << yyy << " " << zzz << " ||" << dis2 << endl;

		}
		in.close();

		/*


		std::ofstream fout5("test2.txt", std::ios_base::app | std::ios_base::out);
		int x, y, z, id, counter,idd;
		std::ifstream in("GPS.txt");
		while (in >> id >> x >> y >> z >> counter){

		idd=projection(x, hyy, z);
		fout5 << idd<<endl;
		}
		*/


	}






	//========================================
	if (imguiButton("Set Random End", m_sposSet))
	{
		//for (int i=1 ; i<=100 ; i++)

		//{
		if (m_sposSet)
		{
			dtStatus status = m_navQuery->findRandomPointAroundCircle(m_startRef, m_spos, m_randomRadius, &m_filter, frand, &m_endRef, m_epos);
			if (dtStatusSucceed(status))
			{
				m_eposSet = true;
				recalc();
			}
		}
		//}
	}

	imguiSeparator();

	if (imguiButton("Make Random Points"))
	{
		m_randPointsInCircle = false;
		m_nrandPoints = 0;
		for (int i = 0; i < MAX_RAND_POINTS; i++)
		{
			float pt[3];
			dtPolyRef ref;
			dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &ref, pt);
			if (dtStatusSucceed(status))
			{
				dtVcopy(&m_randPoints[m_nrandPoints * 3], pt);
				m_nrandPoints++;
			}
		}
	}


	//=========================

	if (imguiButton("show path points "))
		m_nrandPoints5 = 1;

	



	if (imguiButton("memory counter ")){
	 
		std::ofstream fout5("test3.txt", std::ios_base::app | std::ios_base::out);
		//fout5.clear();

		float x, y, z;
		float x2, y2, z2;
		float path_point[3], gps_point[3];
		int counter, id, poly, poly2, d, best_id;



		std::ifstream in("path_points.txt");

		while (in >> poly >> x >> y >> z){
			path_point[0] = x;
			path_point[1] = 0;
			path_point[2] = z;
			d = 1000000;





			std::ifstream in2("gpsIDok.txt");
			while (in2 >> id >> poly2 >> x2 >> y2 >> z2 >> counter){
				gps_point[0] = x2;
				gps_point[1] = 0;
				gps_point[2] = z2;
				if (poly == poly2){
					float dis = dtVdist(path_point, gps_point);
					if (dis < d){
						d = dis;
						best_id = id;

					}
				}


			}
			in2.close();
			a.add_counter(best_id);
			


			fout5 << "\n" << best_id << "  " << "1";


		}



		a.show_counter();
		
	}



	//===================

	if (imguiButton("Show Center of Polygons"))
		m_nrandPoints2 = 1;

	if (imguiButton("Show Crowds targets"))
		m_nrandPoints3 = 1;



	if (imguiButton("Make Random Points Around", m_sposSet))
	{
		if (m_sposSet)
		{
			m_nrandPoints = 0;
			m_randPointsInCircle = true;
			for (int i = 0; i < MAX_RAND_POINTS; i++)
			{
				float pt[3];
				dtPolyRef ref;
				dtStatus status = m_navQuery->findRandomPointAroundCircle(m_startRef, m_spos, m_randomRadius, &m_filter, frand, &ref, pt);
				if (dtStatusSucceed(status))
				{
					dtVcopy(&m_randPoints[m_nrandPoints * 3], pt);
					m_nrandPoints++;
					dtVcopy(m_spos, pt);

					m_sposSet = true;
					recalc();

				}
			}
		}
	}


	imguiSeparator();

	imguiLabel("Include Flags");

	imguiIndent();
	if (imguiCheck("Walk", (m_filter.getIncludeFlags() & SAMPLE_POLYFLAGS_WALK) != 0))
	{
		m_filter.setIncludeFlags(m_filter.getIncludeFlags() ^ SAMPLE_POLYFLAGS_WALK);
		recalc();
	}
	if (imguiCheck("Swim", (m_filter.getIncludeFlags() & SAMPLE_POLYFLAGS_SWIM) != 0))
	{
		m_filter.setIncludeFlags(m_filter.getIncludeFlags() ^ SAMPLE_POLYFLAGS_SWIM);
		recalc();
	}
	if (imguiCheck("Door", (m_filter.getIncludeFlags() & SAMPLE_POLYFLAGS_DOOR) != 0))
	{
		m_filter.setIncludeFlags(m_filter.getIncludeFlags() ^ SAMPLE_POLYFLAGS_DOOR);
		recalc();
	}
	if (imguiCheck("Jump", (m_filter.getIncludeFlags() & SAMPLE_POLYFLAGS_JUMP) != 0))
	{
		m_filter.setIncludeFlags(m_filter.getIncludeFlags() ^ SAMPLE_POLYFLAGS_JUMP);
		recalc();
	}
	imguiUnindent();


	imguiSeparator();
	imguiLabel("Exclude Flags");

	imguiIndent();
	if (imguiCheck("Walk", (m_filter.getExcludeFlags() & SAMPLE_POLYFLAGS_WALK) != 0))
	{
		m_filter.setExcludeFlags(m_filter.getExcludeFlags() ^ SAMPLE_POLYFLAGS_WALK);
		recalc();
	}
	if (imguiCheck("Swim", (m_filter.getExcludeFlags() & SAMPLE_POLYFLAGS_SWIM) != 0))
	{
		m_filter.setExcludeFlags(m_filter.getExcludeFlags() ^ SAMPLE_POLYFLAGS_SWIM);
		recalc();
	}
	if (imguiCheck("Door", (m_filter.getExcludeFlags() & SAMPLE_POLYFLAGS_DOOR) != 0))
	{
		m_filter.setExcludeFlags(m_filter.getExcludeFlags() ^ SAMPLE_POLYFLAGS_DOOR);
		recalc();
	}
	if (imguiCheck("Jump", (m_filter.getExcludeFlags() & SAMPLE_POLYFLAGS_JUMP) != 0))
	{
		m_filter.setExcludeFlags(m_filter.getExcludeFlags() ^ SAMPLE_POLYFLAGS_JUMP);
		recalc();
	}
	imguiUnindent();

	imguiSeparator();
}

void NavMeshTesterTool::handleClick(const float* /*s*/, const float* p, bool shift)
{




	if (shift)
	{
		m_sposSet = true;
		dtVcopy(m_spos, p);
	}
	else
	{
		m_eposSet = true;
		dtVcopy(m_epos, p);
	}

	recalc();


}

void NavMeshTesterTool::handleStep()
{
}

void NavMeshTesterTool::handleToggle()
{
	// TODO: merge separate to a path iterator. Use same code in recalc() too.
	if (m_toolMode != TOOLMODE_PATHFIND_FOLLOW)
		return;

	if (!m_sposSet || !m_eposSet || !m_startRef || !m_endRef)
		return;

	static const float STEP_SIZE = 0.5f;
	static const float SLOP = 0.01f;

	if (m_pathIterNum == 0)
	{

		m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);

		m_nsmoothPath = 0;

		m_pathIterPolyCount = m_npolys;
		if (m_pathIterPolyCount)
			memcpy(m_pathIterPolys, m_polys, sizeof(dtPolyRef)*m_pathIterPolyCount);

		if (m_pathIterPolyCount)
		{
			// Iterate over the path to find smooth path on the detail mesh surface.
			m_navQuery->closestPointOnPoly(m_startRef, m_spos, m_iterPos, 0);
			m_navQuery->closestPointOnPoly(m_pathIterPolys[m_pathIterPolyCount - 1], m_epos, m_targetPos, 0);

			m_nsmoothPath = 0;

			dtVcopy(&m_smoothPath[m_nsmoothPath * 3], m_iterPos);
			m_nsmoothPath++;
		}
	}

	dtVcopy(m_prevIterPos, m_iterPos);

	m_pathIterNum++;

	if (!m_pathIterPolyCount)
		return;

	if (m_nsmoothPath >= MAX_SMOOTH)
		return;

	// Move towards target a small advancement at a time until target reached or
	// when ran out of memory to store the path.

	// Find location to steer towards.
	float steerPos[3];
	unsigned char steerPosFlag;
	dtPolyRef steerPosRef;

	if (!getSteerTarget(m_navQuery, m_iterPos, m_targetPos, SLOP,
		m_pathIterPolys, m_pathIterPolyCount, steerPos, steerPosFlag, steerPosRef,
		m_steerPoints, &m_steerPointCount))
		return;

	dtVcopy(m_steerPos, steerPos);

	bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
	bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

	// Find movement delta.
	float delta[3], len;
	dtVsub(delta, steerPos, m_iterPos);
	len = sqrtf(dtVdot(delta, delta));
	// If the steer target is end of path or off-mesh link, do not move past the location.
	if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
		len = 1;
	else
		len = STEP_SIZE / len;
	float moveTgt[3];
	dtVmad(moveTgt, m_iterPos, delta, len);

	// Move
	float result[3];
	dtPolyRef visited[16];
	int nvisited = 0;
	m_navQuery->moveAlongSurface(m_pathIterPolys[0], m_iterPos, moveTgt, &m_filter,
		result, visited, &nvisited, 16);
	m_pathIterPolyCount = fixupCorridor(m_pathIterPolys, m_pathIterPolyCount, MAX_POLYS, visited, nvisited);
	m_pathIterPolyCount = fixupShortcuts(m_pathIterPolys, m_pathIterPolyCount, m_navQuery);

	float h = 0;
	m_navQuery->getPolyHeight(m_pathIterPolys[0], result, &h);
	result[1] = h;
	dtVcopy(m_iterPos, result);

	// Handle end of path and off-mesh links when close enough.
	if (endOfPath && inRange(m_iterPos, steerPos, SLOP, 1.0f))
	{
		// Reached end of path.
		dtVcopy(m_iterPos, m_targetPos);
		if (m_nsmoothPath < MAX_SMOOTH)
		{
			dtVcopy(&m_smoothPath[m_nsmoothPath * 3], m_iterPos);
			m_nsmoothPath++;
		}
		return;
	}
	else if (offMeshConnection && inRange(m_iterPos, steerPos, SLOP, 1.0f))
	{
		// Reached off-mesh connection.
		float startPos[3], endPos[3];

		// Advance the path up to and over the off-mesh connection.
		dtPolyRef prevRef = 0, polyRef = m_pathIterPolys[0];
		int npos = 0;
		while (npos < m_pathIterPolyCount && polyRef != steerPosRef)
		{
			prevRef = polyRef;
			polyRef = m_pathIterPolys[npos];
			npos++;
		}
		for (int i = npos; i < m_pathIterPolyCount; ++i)
			m_pathIterPolys[i - npos] = m_pathIterPolys[i];
		m_pathIterPolyCount -= npos;

		// Handle the connection.
		dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
		if (dtStatusSucceed(status))
		{
			if (m_nsmoothPath < MAX_SMOOTH)
			{
				dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
				m_nsmoothPath++;
				// Hack to make the dotted path not visible during off-mesh connection.
				if (m_nsmoothPath & 1)
				{
					dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
					m_nsmoothPath++;
				}
			}
			// Move position at the other side of the off-mesh link.
			dtVcopy(m_iterPos, endPos);
			float eh = 0.0f;
			m_navQuery->getPolyHeight(m_pathIterPolys[0], m_iterPos, &eh);
			m_iterPos[1] = eh;
		}
	}

	// Store results.
	if (m_nsmoothPath < MAX_SMOOTH)
	{
		dtVcopy(&m_smoothPath[m_nsmoothPath * 3], m_iterPos);
		m_nsmoothPath++;
	}

}

void NavMeshTesterTool::handleUpdate(const float /*dt*/)
{
	if (m_toolMode == TOOLMODE_PATHFIND_SLICED)
	{
		if (dtStatusInProgress(m_pathFindStatus))
		{
			m_pathFindStatus = m_navQuery->updateSlicedFindPath(1, 0);
		}
		if (dtStatusSucceed(m_pathFindStatus))
		{
			m_navQuery->finalizeSlicedFindPath(m_polys, &m_npolys, MAX_POLYS);
			m_nstraightPath = 0;
			if (m_npolys)
			{


				// In case of partial path, make sure the end point is clamped to the last polygon.
				float epos[3];
				dtVcopy(epos, m_epos);
				if (m_polys[m_npolys - 1] != m_endRef)
					m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], m_epos, epos, 0);

				m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys,
					m_straightPath, m_straightPathFlags,
					m_straightPathPolys, &m_nstraightPath, MAX_POLYS);
			}

			m_pathFindStatus = DT_FAILURE;
		}
	}
}

void NavMeshTesterTool::reset()
{
	m_startRef = 0;
	m_endRef = 0;
	m_npolys = 0;
	m_nstraightPath = 0;
	m_nsmoothPath = 0;
	memset(m_hitPos, 0, sizeof(m_hitPos));
	memset(m_hitNormal, 0, sizeof(m_hitNormal));
	m_distanceToWall = 0;
}


//*************************************************************************************


// vahid 703
void NavMeshTesterTool::testag(dtPolyRef s, float x1, float y1, float z1, dtPolyRef e, float x2, float y2, float z2){


	m_spos[0] = x1;
	m_spos[1] = y1;
	m_spos[2] = z1;

	m_epos[0] = x2;
	m_epos[1] = y2;
	m_epos[2] = z2;



	m_startRef = s;
	m_endRef = e;

	m_sposSet = true;
	m_eposSet = true;
	m_toolMode = TOOLMODE_PATHFIND_FOLLOW;
	recalc();

}

//**************************************************************************************

int NavMeshTesterTool::projection(float x1, float y1, float z1){ //

	//int r;

	m_spos[0] = x1;
	m_spos[1] = hyy;
	m_spos[2] = z1;

	m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);

	// if (m_startRef == 000)
	//	 return 000;
	std::ofstream fout("path_poly.txt", std::ios_base::app | std::ios_base::out);

	fout << m_startRef;
	return m_startRef;
}



//=======================





void NavMeshTesterTool::recalc()
{
	if (!m_navMesh)
		return;

	if (m_sposSet)
		m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
	else
		m_startRef = 0;

	if (m_eposSet)
		m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);
	else
		m_endRef = 0;

	m_pathFindStatus = DT_FAILURE;

	if (m_toolMode == TOOLMODE_PATHFIND_FOLLOW)
	{
		m_pathIterNum = 0;
		if (m_sposSet && m_eposSet && m_startRef && m_endRef)
		{





#ifdef DUMP_REQS
			printf("pi  %f %f %f  %f %f %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif


			//m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
			m_sample->m_ctx->resetLog();
			m_sample->m_ctx->resetTimers();
			// Start the build process.	
			m_sample->m_ctx->startTimer(RC_TIMER_TOTAL);




			if (v_flag){
				v_flag = false;
				m_sample->m_hnav.findPathNav3(m_sample->m_ctx, m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, m_npolys, MAX_POLYS);
			}
			else {
				m_sample->m_hnav.findPathNav(m_sample->m_ctx, m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, m_npolys, MAX_POLYS);


				//-------------------------------------------
				//vahid620



				std::ofstream fout2("path_poly.txt", std::ios_base::app | std::ios_base::out);

				for (int i = 0; i <= m_npolys - 1; i++){

					fout2 << m_polys[i] << "  ";

				}





			}
			//====




			//vahid16
			m_sample->m_ctx->stopTimer(RC_TIMER_TOTAL);
			// Show performance stats.
			duLogBuildTimes(*m_sample->m_ctx, m_sample->m_ctx->getAccumulatedTime(RC_TIMER_TOTAL));



			m_nsmoothPath = 0;

			if (m_npolys)
			{
				// Iterate over the path to find smooth path on the detail mesh surface.
				dtPolyRef polys[MAX_POLYS];
				memcpy(polys, m_polys, sizeof(dtPolyRef)*m_npolys);
				int npolys = m_npolys;

				float iterPos[3], targetPos[3];
				m_navQuery->closestPointOnPoly(m_startRef, m_spos, iterPos, 0);
				m_navQuery->closestPointOnPoly(polys[npolys - 1], m_epos, targetPos, 0);

				static const float STEP_SIZE = 0.5f;
				static const float SLOP = 0.01f;

				m_nsmoothPath = 0;

				dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
				m_nsmoothPath++;

				// Move towards target a small advancement at a time until target reached or
				// when ran out of memory to store the path.
				while (npolys && m_nsmoothPath < MAX_SMOOTH)
				{
					// Find location to steer towards.
					float steerPos[3];
					unsigned char steerPosFlag;
					dtPolyRef steerPosRef;

					if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP,
						polys, npolys, steerPos, steerPosFlag, steerPosRef))
						break;

					bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
					bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

					// Find movement delta.
					float delta[3], len;
					dtVsub(delta, steerPos, iterPos);
					len = dtSqrt(dtVdot(delta, delta));
					// If the steer target is end of path or off-mesh link, do not move past the location.
					if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
						len = 1;
					else
						len = STEP_SIZE / len;
					float moveTgt[3];
					dtVmad(moveTgt, iterPos, delta, len);

					// Move
					float result[3];
					dtPolyRef visited[16];
					int nvisited = 0;
					m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter,
						result, visited, &nvisited, 16);

					npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
					npolys = fixupShortcuts(polys, npolys, m_navQuery);

					float h = 0;
					m_navQuery->getPolyHeight(polys[0], result, &h);
					result[1] = h;
					dtVcopy(iterPos, result);

					// Handle end of path and off-mesh links when close enough.
					if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
					{
						// Reached end of path.
						dtVcopy(iterPos, targetPos);
						if (m_nsmoothPath < MAX_SMOOTH)
						{
							dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
							m_nsmoothPath++;
						}
						break;
					}
					else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f))
					{
						// Reached off-mesh connection.
						float startPos[3], endPos[3];

						// Advance the path up to and over the off-mesh connection.
						dtPolyRef prevRef = 0, polyRef = polys[0];
						int npos = 0;
						while (npos < npolys && polyRef != steerPosRef)
						{
							prevRef = polyRef;
							polyRef = polys[npos];
							npos++;
						}



						for (int i = npos; i < npolys; ++i)
							polys[i - npos] = polys[i];
						npolys -= npos;


						// Handle the connection.
						dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
						if (dtStatusSucceed(status))
						{
							if (m_nsmoothPath < MAX_SMOOTH)
							{
								dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
								m_nsmoothPath++;
								// Hack to make the dotted path not visible during off-mesh connection.
								if (m_nsmoothPath & 1)
								{
									dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
									m_nsmoothPath++;
								}
							}
							// Move position at the other side of the off-mesh link.
							dtVcopy(iterPos, endPos);
							float eh = 0.0f;
							m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
							iterPos[1] = eh;
						}
					}


					// Store results.
					if (m_nsmoothPath < MAX_SMOOTH)
					{
						dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
						m_nsmoothPath++;

					}


					//	for (int i = 0; i < m_nsmoothPath; i++)
					//	fout3 << "\n" << m_nsmoothPath << "   ---    " << m_smoothPath[i * 3] << "  " << m_smoothPath[i * 3 + 1] + 0.1f << "  " << m_smoothPath[i * 3 + 2];

				}

			}

		}
		else
		{
			m_npolys = 0;
			m_nsmoothPath = 0;
		}

		//vahid501



		std::ofstream fout4("path_points.txt", std::ios_base::app | std::ios_base::out);
		fout4.clear();
		dtPolyRef re;

		float pathid[3];

		for (int i = 0; i < m_nsmoothPath; i++){
			if (i % 5 == 0)
				pathid[0] = m_smoothPath[i * 3];
			pathid[1] = m_smoothPath[i * 3 + 1];
			pathid[2] = m_smoothPath[i * 3 + 2];

			m_navQuery->findNearestPoly(pathid, m_polyPickExt, &m_filter, &re, 0);

			fout4 << "\n" << re << "  " << m_smoothPath[i * 3] << "  " << m_smoothPath[i * 3 + 1] + 0.1f << "  " << m_smoothPath[i * 3 + 2];
		}
		fout4.close();

		//================
		/*int id1, poly1, counter = 1,c;
		float x, y, z;

		std::ifstream in("gpsID.txt");
		std::ofstream fout("gpsID2.txt", std::ios_base::app | std::ios_base::out);

		while (in >> id1 >> poly1 >> x >> y >> z >> c){
			pathid[0] = x;
			pathid[1] = y;
			pathid[2] = z;
			m_navQuery->findNearestPoly(pathid, m_polyPickExt, &m_filter, &re, 0);
			if (re != 0){
				fout << "\n" << counter << "  " << re << "  " << x << "  " << y << "  " << z <<"  "<<c;
				counter++;
			}
		}
		*/
		//===================








		//==========================================
		/*

		std::ofstream fout3("logcol2.txt", std::ios_base::app | std::ios_base::out);
		float p1[3],p2[3];
		float sum = 0;

		for (int i = 0; i < m_nsmoothPath; i++){

		p1[0] = m_smoothPath[i * 3];
		p1[1] = m_smoothPath[i * 3+1];
		p1[2] = m_smoothPath[i * 3+2];

		p2[0] = m_smoothPath[(i+1) * 3];
		p2[1] = m_smoothPath[(i + 1) * 3 + 1];
		p2[2] = m_smoothPath[(i + 1) * 3 + 2];

		sum+=dtVdist(p1,p2);



		}
		int start = sum;
		int end = sum + 50;
		float ran = 0.0f;

		//ran = abs((rand() % end)-start);

		ran = rand() % 50 + frand()+1;

		fout3 << "\n" << sum<<"   " <<sum+ran;
		*/
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	else if (m_toolMode == TOOLMODE_PATHFIND_STRAIGHT)
	{
		if (m_sposSet && m_eposSet && m_startRef && m_endRef)
		{
#ifdef DUMP_REQS
			printf("ps  %f %f %f  %f %f %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);

			m_nstraightPath = 0;
			if (m_npolys)
			{
				// In case of partial path, make sure the end point is clamped to the last polygon.
				float epos[3];
				dtVcopy(epos, m_epos);
				if (m_polys[m_npolys - 1] != m_endRef)
					m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], m_epos, epos, 0);

				m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys,
					m_straightPath, m_straightPathFlags,
					m_straightPathPolys, &m_nstraightPath, MAX_POLYS, m_straightPathOptions);
			}
		}
		else
		{
			m_npolys = 0;
			m_nstraightPath = 0;
		}
	}







	else if (m_toolMode == TOOLMODE_PATHFIND_SLICED)
	{
		if (m_sposSet && m_eposSet && m_startRef && m_endRef)
		{
#ifdef DUMP_REQS
			printf("ps  %f %f %f  %f %f %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			m_npolys = 0;
			m_nstraightPath = 0;

			m_pathFindStatus = m_navQuery->initSlicedFindPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter);
		}
		else
		{
			m_npolys = 0;
			m_nstraightPath = 0;
		}
	}
	else if (m_toolMode == TOOLMODE_RAYCAST)
	{
		m_nstraightPath = 0;
		if (m_sposSet && m_eposSet && m_startRef)
		{
#ifdef DUMP_REQS
			printf("rc  %f %f %f  %f %f %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			float t = 0;
			m_npolys = 0;
			m_nstraightPath = 2;
			m_straightPath[0] = m_spos[0];
			m_straightPath[1] = m_spos[1];
			m_straightPath[2] = m_spos[2];
			m_navQuery->raycast(m_startRef, m_spos, m_epos, &m_filter, &t, m_hitNormal, m_polys, &m_npolys, MAX_POLYS);
			if (t > 1)
			{
				// No hit
				dtVcopy(m_hitPos, m_epos);
				m_hitResult = false;
			}
			else
			{
				// Hit
				dtVlerp(m_hitPos, m_spos, m_epos, t);
				m_hitResult = true;
			}
			// Adjust height.
			if (m_npolys > 0)
			{
				float h = 0;
				m_navQuery->getPolyHeight(m_polys[m_npolys - 1], m_hitPos, &h);
				m_hitPos[1] = h;
			}
			dtVcopy(&m_straightPath[3], m_hitPos);
		}
	}
	else if (m_toolMode == TOOLMODE_DISTANCE_TO_WALL)
	{
		m_distanceToWall = 0;
		if (m_sposSet && m_startRef)
		{
#ifdef DUMP_REQS
			printf("dw  %f %f %f  %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], 100.0f,
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			m_distanceToWall = 0.0f;
			m_navQuery->findDistanceToWall(m_startRef, m_spos, 100.0f, &m_filter, &m_distanceToWall, m_hitPos, m_hitNormal);
		}
	}
	else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_CIRCLE)
	{
		if (m_sposSet && m_startRef && m_eposSet)
		{
			const float dx = m_epos[0] - m_spos[0];
			const float dz = m_epos[2] - m_spos[2];
			float dist = sqrtf(dx*dx + dz*dz);
#ifdef DUMP_REQS
			printf("fpc  %f %f %f  %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], dist,
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			m_navQuery->findPolysAroundCircle(m_startRef, m_spos, dist, &m_filter,
				m_polys, m_parent, 0, &m_npolys, MAX_POLYS);

		}
	}
	else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_SHAPE)
	{
		if (m_sposSet && m_startRef && m_eposSet)
		{
			const float nx = (m_epos[2] - m_spos[2])*0.25f;
			const float nz = -(m_epos[0] - m_spos[0])*0.25f;
			const float agentHeight = m_sample ? m_sample->getAgentHeight() : 0;

			m_queryPoly[0] = m_spos[0] + nx*1.2f;
			m_queryPoly[1] = m_spos[1] + agentHeight / 2;
			m_queryPoly[2] = m_spos[2] + nz*1.2f;

			m_queryPoly[3] = m_spos[0] - nx*1.3f;
			m_queryPoly[4] = m_spos[1] + agentHeight / 2;
			m_queryPoly[5] = m_spos[2] - nz*1.3f;

			m_queryPoly[6] = m_epos[0] - nx*0.8f;
			m_queryPoly[7] = m_epos[1] + agentHeight / 2;
			m_queryPoly[8] = m_epos[2] - nz*0.8f;

			m_queryPoly[9] = m_epos[0] + nx;
			m_queryPoly[10] = m_epos[1] + agentHeight / 2;
			m_queryPoly[11] = m_epos[2] + nz;

#ifdef DUMP_REQS
			printf("fpp  %f %f %f  %f %f %f  %f %f %f  %f %f %f  0x%x 0x%x\n",
				m_queryPoly[0], m_queryPoly[1], m_queryPoly[2],
				m_queryPoly[3], m_queryPoly[4], m_queryPoly[5],
				m_queryPoly[6], m_queryPoly[7], m_queryPoly[8],
				m_queryPoly[9], m_queryPoly[10], m_queryPoly[11],
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			m_navQuery->findPolysAroundShape(m_startRef, m_queryPoly, 4, &m_filter,
				m_polys, m_parent, 0, &m_npolys, MAX_POLYS);
		}
	}
	else if (m_toolMode == TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD)
	{
		if (m_sposSet && m_startRef)
		{

			//
#ifdef DUMP_REQS
			printf("fln  %f %f %f  %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], m_neighbourhoodRadius,
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			m_navQuery->findLocalNeighbourhood(m_startRef, m_spos, m_neighbourhoodRadius, &m_filter,
				m_polys, m_parent, &m_npolys, MAX_POLYS);
		}
	}
}




void NavMeshTesterTool::handleRender()
{
	DebugDrawGL dd;
	DebugDrawGL vv;
	static const unsigned int startCol = duRGBA(128, 25, 0, 192);
	static const unsigned int endCol = duRGBA(51, 102, 0, 129);
	static const unsigned int pathCol = duRGBA(0, 0, 0, 64);

	const float agentRadius = m_sample->getAgentRadius();
	const float agentHeight = m_sample->getAgentHeight();
	const float agentClimb = m_sample->getAgentClimb();

	dd.depthMask(false);
	if (m_sposSet)
		drawAgent(m_spos, agentRadius, agentHeight, agentClimb, startCol);
	if (m_eposSet)
		drawAgent(m_epos, agentRadius, agentHeight, agentClimb, endCol);
	dd.depthMask(true);

	if (!m_navMesh)
	{
		return;
	}

	if (m_toolMode == TOOLMODE_PATHFIND_FOLLOW)
	{
		duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);
		duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_endRef, endCol);

		if (m_npolys)
		{
			for (int i = 0; i < m_npolys; ++i)
			{
				if (m_polys[i] == m_startRef || m_polys[i] == m_endRef)
					continue;
				duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
			}
		}


		//	std::ofstream fout3("logcol.txt", std::ios_base::app | std::ios_base::out);
		//fout3 << "\n" << m_nsmoothPath;



		if (m_nsmoothPath)
		{
			dd.depthMask(false);

			// vahid_len ==>> path color 
			//std::ofstream fout3("logcol.txt", std::ios_base::app | std::ios_base::out);
			const unsigned int spathCol = duRGBA(0, 0, 255, 220);
			dd.begin(DU_DRAW_LINES, 3.0f);



			//std::ofstream fout3("logcol.txt", std::ios_base::app | std::ios_base::out);
			//	fout3 << "\n" << m_nsmoothPath;




			for (int i = 0; i < m_nsmoothPath; ++i)
				//{
				dd.vertex(m_smoothPath[i * 3], m_smoothPath[i * 3 + 1] + 0.1f, m_smoothPath[i * 3 + 2], spathCol);
			//std::ofstream fout3("logcol.txt", std::ios_base::app | std::ios_base::out);
			//fout3 << "\n" << m_nsmoothPath << "   " << m_smoothPath[i * 3] << "  " << m_smoothPath[i * 3 + 1] + 0.1f << "  " << m_smoothPath[i * 3 + 2];
			//	}
			dd.end();
			dd.depthMask(true);
		}

		if (m_pathIterNum)
		{
			duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_pathIterPolys[0], duRGBA(255, 255, 255, 128));

			dd.depthMask(false);
			dd.begin(DU_DRAW_LINES, 1.0f);

			const unsigned int prevCol = duRGBA(255, 192, 0, 220);
			const unsigned int curCol = duRGBA(255, 255, 255, 220);
			const unsigned int steerCol = duRGBA(0, 192, 255, 220);

			dd.vertex(m_prevIterPos[0], m_prevIterPos[1] - 0.3f, m_prevIterPos[2], prevCol);
			dd.vertex(m_prevIterPos[0], m_prevIterPos[1] + 0.3f, m_prevIterPos[2], prevCol);

			dd.vertex(m_iterPos[0], m_iterPos[1] - 0.3f, m_iterPos[2], curCol);
			dd.vertex(m_iterPos[0], m_iterPos[1] + 0.3f, m_iterPos[2], curCol);

			dd.vertex(m_prevIterPos[0], m_prevIterPos[1] + 0.3f, m_prevIterPos[2], prevCol);
			dd.vertex(m_iterPos[0], m_iterPos[1] + 0.3f, m_iterPos[2], prevCol);

			dd.vertex(m_prevIterPos[0], m_prevIterPos[1] + 0.3f, m_prevIterPos[2], steerCol);
			dd.vertex(m_steerPos[0], m_steerPos[1] + 0.3f, m_steerPos[2], steerCol);

			for (int i = 0; i < m_steerPointCount - 1; ++i)
			{
				dd.vertex(m_steerPoints[i * 3 + 0], m_steerPoints[i * 3 + 1] + 0.2f, m_steerPoints[i * 3 + 2], duDarkenCol(steerCol));
				dd.vertex(m_steerPoints[(i + 1) * 3 + 0], m_steerPoints[(i + 1) * 3 + 1] + 0.2f, m_steerPoints[(i + 1) * 3 + 2], duDarkenCol(steerCol));
			}

			dd.end();
			dd.depthMask(true);
		}
	}
	else if (m_toolMode == TOOLMODE_PATHFIND_STRAIGHT ||
		m_toolMode == TOOLMODE_PATHFIND_SLICED)
	{
		duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);
		duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_endRef, endCol);

		if (m_npolys)
		{
			for (int i = 0; i < m_npolys; ++i)
			{
				if (m_polys[i] == m_startRef || m_polys[i] == m_endRef)
					continue;
				duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
			}
		}

		if (m_nstraightPath)
		{
			dd.depthMask(false);
			const unsigned int spathCol = duRGBA(64, 16, 0, 220);
			const unsigned int offMeshCol = duRGBA(128, 96, 0, 220);
			dd.begin(DU_DRAW_LINES, 2.0f);
			for (int i = 0; i < m_nstraightPath - 1; ++i)
			{
				unsigned int col = 0;
				if (m_straightPathFlags[i] & DT_STRAIGHTPATH_OFFMESH_CONNECTION)
					col = offMeshCol;
				else
					col = spathCol;

				dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], col);
				dd.vertex(m_straightPath[(i + 1) * 3], m_straightPath[(i + 1) * 3 + 1] + 0.4f, m_straightPath[(i + 1) * 3 + 2], col);




			}
			dd.end();
			dd.begin(DU_DRAW_POINTS, 6.0f);
			for (int i = 0; i < m_nstraightPath; ++i)
			{
				unsigned int col = 0;
				if (m_straightPathFlags[i] & DT_STRAIGHTPATH_START)
					col = startCol;
				else if (m_straightPathFlags[i] & DT_STRAIGHTPATH_START)
					col = endCol;
				else if (m_straightPathFlags[i] & DT_STRAIGHTPATH_OFFMESH_CONNECTION)
					col = offMeshCol;
				else
					col = spathCol;
				dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], col);
			}
			dd.end();
			dd.depthMask(true);
		}
	}
	else if (m_toolMode == TOOLMODE_RAYCAST)
	{
		duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);

		if (m_nstraightPath)
		{
			for (int i = 1; i < m_npolys; ++i)
				duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);

			dd.depthMask(false);
			const unsigned int spathCol = m_hitResult ? duRGBA(64, 16, 0, 220) : duRGBA(240, 240, 240, 220);
			dd.begin(DU_DRAW_LINES, 2.0f);
			for (int i = 0; i < m_nstraightPath - 1; ++i)
			{
				dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], spathCol);
				dd.vertex(m_straightPath[(i + 1) * 3], m_straightPath[(i + 1) * 3 + 1] + 0.4f, m_straightPath[(i + 1) * 3 + 2], spathCol);
			}
			dd.end();
			dd.begin(DU_DRAW_POINTS, 4.0f);
			for (int i = 0; i < m_nstraightPath; ++i)
				dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], spathCol);
			dd.end();



			if (m_hitResult)
			{
				const unsigned int hitCol = duRGBA(0, 0, 0, 128);
				dd.begin(DU_DRAW_LINES, 2.0f);
				dd.vertex(m_hitPos[0], m_hitPos[1] + 0.4f, m_hitPos[2], hitCol);
				dd.vertex(m_hitPos[0] + m_hitNormal[0] * agentRadius,
					m_hitPos[1] + 0.4f + m_hitNormal[1] * agentRadius,
					m_hitPos[2] + m_hitNormal[2] * agentRadius, hitCol);
				dd.end();
			}
			dd.depthMask(true);
		}
	}
	else if (m_toolMode == TOOLMODE_DISTANCE_TO_WALL)
	{
		duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);
		dd.depthMask(false);
		duDebugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], m_distanceToWall, duRGBA(64, 16, 0, 220), 2.0f);
		dd.begin(DU_DRAW_LINES, 3.0f);
		dd.vertex(m_hitPos[0], m_hitPos[1] + 0.02f, m_hitPos[2], duRGBA(0, 0, 0, 192));
		dd.vertex(m_hitPos[0], m_hitPos[1] + agentHeight, m_hitPos[2], duRGBA(0, 0, 0, 192));
		dd.end();
		dd.depthMask(true);
	}

	else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_CIRCLE)
	{



		for (int i = 0; i < m_npolys; ++i)
		{
			duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
			dd.depthMask(false);
			if (m_parent[i])
			{
				float p0[3], p1[3];
				dd.depthMask(false);
				getPolyCenter(m_navMesh, m_parent[i], p0);
				getPolyCenter(m_navMesh, m_polys[i], p1);
				duDebugDrawArc(&dd, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0.25f, 0.0f, 0.4f, duRGBA(0, 0, 0, 128), 2.0f);
				dd.depthMask(true);
			}
			dd.depthMask(true);
		}

		if (m_sposSet && m_eposSet)
		{
			dd.depthMask(false);
			const float dx = m_epos[0] - m_spos[0];
			const float dz = m_epos[2] - m_spos[2];
			const float dist = sqrtf(dx*dx + dz*dz);
			duDebugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], dist, duRGBA(64, 16, 0, 220), 2.0f);
			dd.depthMask(true);
		}

	}
	else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_SHAPE)
	{
		for (int i = 0; i < m_npolys; ++i)
		{
			duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
			dd.depthMask(false);
			if (m_parent[i])
			{
				float p0[3], p1[3];
				dd.depthMask(false);
				getPolyCenter(m_navMesh, m_parent[i], p0);

				//		std::ofstream fout("center.txt", std::ios_base::app | std::ios_base::out);

				//===vahid----



				getPolyCenter(m_navMesh, m_polys[i], p1);

				duDebugDrawArc(&dd, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0.25f, 0.0f, 0.4f, duRGBA(0, 0, 0, 128), 2.0f);
				dd.depthMask(true);
				//	fout << "\n" << p0[0]<<" "<<p0[1]<<" "<< p0[2];


			}
			dd.depthMask(true);
		}

		if (m_sposSet && m_eposSet)
		{
			dd.depthMask(false);
			const unsigned int col = duRGBA(64, 16, 0, 220);
			dd.begin(DU_DRAW_LINES, 2.0f);
			for (int i = 0, j = 3; i < 4; j = i++)
			{
				const float* p0 = &m_queryPoly[j * 3];
				const float* p1 = &m_queryPoly[i * 3];
				dd.vertex(p0, col);
				dd.vertex(p1, col);
			}
			dd.end();
			dd.depthMask(true);
		}
	}
	else if (m_toolMode == TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD)
	{
		for (int i = 0; i < m_npolys; ++i)
		{
			duDebugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
			dd.depthMask(false);
			if (m_parent[i])
			{
				float p0[3], p1[3];
				dd.depthMask(false);
				getPolyCenter(m_navMesh, m_parent[i], p0);
				getPolyCenter(m_navMesh, m_polys[i], p1);
				duDebugDrawArc(&dd, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0.25f, 0.0f, 0.4f, duRGBA(0, 0, 0, 128), 2.0f);
				dd.depthMask(true);
			}

			static const int MAX_SEGS = DT_VERTS_PER_POLYGON * 4;
			float segs[MAX_SEGS * 6];
			dtPolyRef refs[MAX_SEGS];
			memset(refs, 0, sizeof(dtPolyRef)*MAX_SEGS);
			int nsegs = 0;
			m_navQuery->getPolyWallSegments(m_polys[i], &m_filter, segs, refs, &nsegs, MAX_SEGS);
			dd.begin(DU_DRAW_LINES, 2.0f);
			for (int j = 0; j < nsegs; ++j)
			{
				const float* s = &segs[j * 6];

				// Skip too distant segments.
				float tseg;
				float distSqr = dtDistancePtSegSqr2D(m_spos, s, s + 3, tseg);
				if (distSqr > dtSqr(m_neighbourhoodRadius))
					continue;

				float delta[3], norm[3], p0[3], p1[3];
				dtVsub(delta, s + 3, s);
				dtVmad(p0, s, delta, 0.5f);
				norm[0] = delta[2];
				norm[1] = 0;
				norm[2] = -delta[0];
				dtVnormalize(norm);
				dtVmad(p1, p0, norm, agentRadius*0.5f);

				// Skip backfacing segments.
				if (refs[j])
				{
					unsigned int col = duRGBA(255, 255, 255, 32);
					dd.vertex(s[0], s[1] + agentClimb, s[2], col);
					dd.vertex(s[3], s[4] + agentClimb, s[5], col);
				}
				else
				{
					unsigned int col = duRGBA(192, 32, 16, 192);
					if (dtTriArea2D(m_spos, s, s + 3) < 0.0f)
						col = duRGBA(96, 32, 16, 192);

					dd.vertex(p0[0], p0[1] + agentClimb, p0[2], col);
					dd.vertex(p1[0], p1[1] + agentClimb, p1[2], col);

					dd.vertex(s[0], s[1] + agentClimb, s[2], col);
					dd.vertex(s[3], s[4] + agentClimb, s[5], col);
				}
			}

			dd.end();

			dd.depthMask(true);
		}

		if (m_sposSet)
		{
			dd.depthMask(false);
			duDebugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], m_neighbourhoodRadius, duRGBA(64, 16, 0, 220), 2.0f);
			dd.depthMask(true);
		}
	}

	if (m_nrandPoints > 0)
	{
		dd.begin(DU_DRAW_POINTS, 6.0f);
		for (int i = 0; i < m_nrandPoints; i++)
		{
			const float* p = &m_randPoints[i * 3];
			dd.vertex(p[0], p[1] + 0.1f, p[2], duRGBA(0, 0, 0, 192));
			//dd.vertex(16.4598, 14.9973, -87.0413, duRGBA(220,32,16,192));

		}
		dd.end();

		if (m_randPointsInCircle && m_sposSet)
		{
			duDebugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], m_randomRadius, duRGBA(64, 16, 0, 220), 2.0f);
		}
	}

	//==================vahid==================================================================










	if (m_nrandPoints4> 0)
	{


		//==================== random point    okk ================================


		/*
		dd.begin(DU_DRAW_POINTS, 6.0f);
		//float x,x2,y2,z2,y,z;
		float x, y, z;
		int id;
		std::ifstream in("Random_Points.txt");
		while (in >> id >> x >> y >> z)
		dd.vertex(x, y, z, duRGBA(220, 32, 16, 192));

		dd.end();

		in.close();

		*/

		//==================== random point    okk ================================



		//	std::ifstream p1("gpsID5.txt");
		//	float a1, b1, c1, d1, e1, f1, g1;

		//	while (p1 >> a1 >> b1 >> c1)
		//	dd.vertex(a1, b1, c1, duRGBA(76, 76, 255, 255));





		//=================================================================



		/*

		int id,id2;
		//  std::ifstream in("Random_Points.txt");

		std::ifstream in("v1_Center Of polygons.txt");
		while(in>>id>>x>>y>>z)  {


		dd.vertex(x, y,z, duRGBA(220,32,16,192));
		//40.9848 10.1973 -0.541272


		}

		std::ifstream in2("v2_Inter-edges.txt");
		while(in2>>id2>>x2>>y2>>z2)  {


		dd.vertex(x2, y2,z2, duRGBA(16,32,220,192));

		}

		*/


	}




	if (m_nrandPoints5 > 0)
	{

		int id;
		float x, y, z;
		int counter = 0;
		dd.begin(DU_DRAW_POINTS, 6.0f);
		std::ifstream f("path_points.txt");

		while (f >> id >> x >> y >> z){

			dd.vertex(x, y, z, duRGBA(220, 32, 16, 192));

		}
		dd.end();

	}




	//=====
	if (m_nrandPoints2 > 0)
	{

		/*
		float x, y, z;
		int id;
		std::ifstream in("v1_Center Of polygons.txt");
		while (in >> id >> x >> y >> z)
		dd.vertex(x, y, z, duRGBA(220, 32, 16, 192));
		*/




		// for size=1   ==> step = 3



		//	vv.begin(DU_DRAW_LINES);









		//pathid[0] = x;
		//pathid[1] = y;
		//pathid[2] = z;
		//m_navQuery->findNearestPoly(pathid, m_polyPickExt, &m_filter, &re, 0);
		//if (re>0)




		//duDebugDraw* dd;

		//dd->begin(DU_DRAW_TRIS);




		//==========================================START OK===================================================

		/*
		float x, y, z;
		int counter, id;
		std::ifstream in("newgps.txt");
		//while (in >>id>> x >> y >> z >> counter){
		while (in >> id >> counter >> x >> y >> z){


		unsigned int color = duIntToCol(id, 192);

		//check the background color

		dd.begin(DU_DRAW_TRIS);


		float va[3];
		va[0] = x;
		va[1] = y;
		va[2] = z;


		float vb[3];
		vb[0] = x - size;
		vb[1] = y;
		vb[2] = z;

		float vc[3];
		vc[0] = x - blueStep;
		vc[1] = y;
		vc[2] = z + size;

		dd.vertex(va, color);
		dd.vertex(vb, color);
		dd.vertex(vc, color);
		dd.end();


		//==========

		dd.begin(DU_DRAW_TRIS);

		va[0] = x;
		va[1] = y;
		va[2] = z;



		vb[0] = x - blueStep;
		vb[1] = y;
		vb[2] = z + size;

		vc[0] = x + blueStep;
		vc[1] = y;
		vc[2] = z + size;


		dd.vertex(va, color);
		dd.vertex(vb, color);
		dd.vertex(vc, color);

		dd.end();

		//================


		dd.begin(DU_DRAW_TRIS);

		va[0] = x;
		va[1] = y;
		va[2] = z;



		vc[0] = x + size;
		vc[1] = y;
		vc[2] = z;

		vb[0] = x + blueStep;
		vb[1] = y;
		vb[2] = z + size;


		dd.vertex(va, color);
		dd.vertex(vb, color);
		dd.vertex(vc, color);

		dd.end();

		//======================

		dd.begin(DU_DRAW_TRIS);

		va[0] = x;
		va[1] = y;
		va[2] = z;



		vb[0] = x + size;
		vb[1] = y;
		vb[2] = z;

		vc[0] = x + blueStep;
		vc[1] = y;
		vc[2] = z - size;


		dd.vertex(va, color);
		dd.vertex(vb, color);
		dd.vertex(vc, color);

		dd.end();

		//======================



		dd.begin(DU_DRAW_TRIS);

		va[0] = x;
		va[1] = y;
		va[2] = z;



		vc[0] = x - blueStep;
		vc[1] = y;
		vc[2] = z - size;

		vb[0] = x + blueStep;
		vb[1] = y;
		vb[2] = z - size;


		dd.vertex(va, color);
		dd.vertex(vb, color);
		dd.vertex(vc, color);

		dd.end();

		//======================

		dd.begin(DU_DRAW_TRIS);

		va[0] = x;
		va[1] = y;
		va[2] = z;



		vb[0] = x - blueStep;
		vb[1] = y;
		vb[2] = z - size;

		vc[0] = x - size;
		vc[1] = y;
		vc[2] = z;


		dd.vertex(va, color);
		dd.vertex(vb, color);
		dd.vertex(vc, color);

		dd.end();

		//======================




		dd.begin(DU_DRAW_POINTS, 4.0f);

		dd.vertex(x, y, z, duRGBA(76, 76, 82, 255));

		dd.vertex(x + size, y, z, duRGBA(76, 76, 82, 255));
		dd.vertex(x - size, y, z, duRGBA(76, 76, 82, 255));

		dd.vertex(x + blueStep, y, z + size, duRGBA(76, 76, 82, 255));
		dd.vertex(x - blueStep, y, z + size, duRGBA(76, 76, 82, 255));

		dd.vertex(x + blueStep, y, z - size, duRGBA(76, 76, 82, 255));
		dd.vertex(x - blueStep, y, z - size, duRGBA(76, 76, 82, 255));
		dd.end();







		}



		//	dd.begin(DU_DRAW_POINTS, 9.0f);
		//
		//	dd.vertex(0,-5, 0, duRGBA(76, 76, 182, 255));
		//dd.vertex(0, -5, 10, duRGBA(176, 76, 82, 255));
		//	dd.vertex(0, -5, -10, duRGBA(76, 176, 82, 255));

		//	dd.end();




		*/


		//==============================END OK==========================================================

		/*

		float x=32, hyy=0, z=-22;

		dd.begin(DU_DRAW_POINTS, 9.0f);

		dd.vertex(x, hyy, z, duRGBA(76, 76, 82, 255));

		dd.vertex(x + size, hyy, z, duRGBA(176, 76, 82, 255));
		dd.vertex(x - size, hyy, z, duRGBA(76, 176, 82, 255));

		dd.vertex(x + blueStep, hyy, z + size, duRGBA(76, 76, 182, 255));
		dd.vertex(x - blueStep, hyy, z + size, duRGBA(0, 0, 0, 255));

		dd.vertex(x + blueStep, hyy, z - size, duRGBA(76, 76, 82, 255));
		dd.vertex(x - blueStep, hyy, z - size, duRGBA(76, 76, 82, 255));
		dd.end();

		*/

		//--------------





		//================================== OK hegxacon with line======START===============================



		float x, y, z;
		int counter, id, poly,c;
		std::ifstream in("gpsIDok.txt");
		
		while (in >> id >> poly >> x >> y >> z >> c){
			//while (in >> id >>counter>> x >> y >> z ){

			y = 3;

			float pathid[3];
			dtPolyRef re;

			GPS_PRINT(x, y, z);

		


		//	if (id == 461 || id == 737 || id == 400 || id == 339 || id == 284 || id == 242 || id == 11 || id == 4 || id == 10 || id == 3 || id == 9 || id == 2 || id == 8 || id == 20 || id == 33 || id == 19 || id == 32 || id == 18 || id == 31 || id == 17 || id == 30 || id == 16 || id == 29 || id == 15 || id == 28 || id == 14 || id == 27 || id == 51 || id == 74 || id == 97 || id == 120 || id == 143 || id == 166 || id == 190 || id == 214 || id == 244 || id == 263 || id == 243 || id == 262 || id == 312 || id == 370 || id == 431 || id == 492 || id == 553 || id == 614 || id == 674 || id == 728 || id == 778 || id == 834 || id == 892 || id == 953 || id == 983 || id == 952 || id == 982 || id == 1013 || id == 981 || id == 1012 || id == 980 || id == 1011 || id == 979 || id == 1010 || id == 1040 || id == 1009 || id == 1039 || id == 1068 || id == 1097 || id == 1126 || id == 1155 || id == 1185 || id == 1154 || id == 1184 || id == 1214 || id == 1183 || id == 1213 || id == 1182 || id == 1212 || id == 1181 || id == 1211 || id == 1241 || id == 1210 || id == 1240 || id == 1209 || id == 1179 || id == 1149 || id == 1180 || id == 1150 || id == 1121 || id == 1151 || id == 1122 || id == 1152 || id == 1123 || id == 1153 || id == 1124 || id == 1095 || id == 1125 || id == 1096 || id == 1067 || id == 1038 || id == 1008 || id == 977 || id == 947 || id == 978 || id == 948 || id == 918 || id == 949 || id == 919 || id == 950 || id == 920 || id == 951 || id == 921 || id == 891 || id == 833 || id == 862 || id == 890 || id == 861 || id == 889 || id == 860 || id == 888 || id == 859 || id == 887 || id == 917 || id == 886 || id == 916 || id == 946 || id == 976 || id == 1007 || id == 1037 || id == 1066 || id == 1036 || id == 1065 || id == 1094 || id == 1064 || id == 1093 || id == 1063 || id == 1092 || id == 1062 || id == 1091 || id == 1120 || id == 1090 || id == 1061 || id == 1032 || id == 1002 || id == 1033 || id == 1003 || id == 1034 || id == 1004 || id == 1035 || id == 1005 || id == 974 || id == 1006 || id == 975 || id == 945 || id == 915 || id == 885 || id == 857 || id == 828 || id == 858 || id == 829 || id == 801 || id == 830 || id == 802 || id == 831 || id == 803 || id == 832 || id == 804 || id == 777 || id == 727 || id == 752 || id == 776 || id == 751 || id == 775 || id == 750 || id == 774 || id == 749 || id == 773 || id == 800 || id == 772 || id == 799 || id == 827 || id == 856 || id == 826 || id == 798 || id == 770 || id == 746 || id == 771 || id == 747 || id == 722 || id == 748 || id == 723 || id == 700 || id == 724 || id == 701 || id == 725 || id == 702 || id == 726 || id == 703 || id == 673 || id == 613 || id == 643 || id == 672 || id == 642 || id == 671 || id == 641 || id == 670 || id == 640 || id == 669 || id == 699 || id == 668 || id == 698 || id == 721 || id == 697 || id == 720 || id == 696 || id == 745 || id == 797 || id == 855 || id == 914 || id == 944 || id == 913 || id == 943 || id == 973 || id == 942 || id == 972 || id == 941 || id == 971 || id == 940 || id == 970 || id == 1001 || id == 1031 || id == 1000 || id == 1030 || id == 999 || id == 937 || id == 968 || id == 938 || id == 969 || id == 939 || id == 909 || id == 881 || id == 910 || id == 882 || id == 911 || id == 883 || id == 912 || id == 884 || id == 825 || id == 854 || id == 824 || id == 853 || id == 823 || id == 852 || id == 822 || id == 851 || id == 880 || id == 908 || id == 879 || id == 907 || id == 878 || id == 819 || id == 849 || id == 820 || id == 850 || id == 821 || id == 793 || id == 766 || id == 794 || id == 767 || id == 795 || id == 768 || id == 796 || id == 769 || id == 744 || id == 719 || id == 743 || id == 718 || id == 742 || id == 717 || id == 741 || id == 716 || id == 740 || id == 765 || id == 792 || id == 764 || id == 791 || id == 763 || id == 738 || id == 714 || id == 739 || id == 715 || id == 691 || id == 661 || id == 692 || id == 662 || id == 693 || id == 663 || id == 694 || id == 664 || id == 695 || id == 665 || id == 636 || id == 666 || id == 637 || id == 667 || id == 638 || id == 608 || id == 639 || id == 609 || id == 579 || id == 610 || id == 580 || id == 611 || id == 581 || id == 612 || id == 582 || id == 552 || id == 521 || id == 551 || id == 520 || id == 550 || id == 519 || id == 549 || id == 518 || id == 548 || id == 578 || id == 547 || id == 577 || id == 607 || id == 576 || id == 606 || id == 575 || id == 605 || id == 635 || id == 604 || id == 634 || id == 603 || id == 633 || id == 602 || id == 632 || id == 601 || id == 631 || id == 660 || id == 690 || id == 659 || id == 689 || id == 713 || id == 688 || id == 628 || id == 658 || id == 629 || id == 599 || id == 630 || id == 600 || id == 570 || id == 540 || id == 571 || id == 541 || id == 572 || id == 542 || id == 573 || id == 543 || id == 574 || id == 544 || id == 514 || id == 545 || id == 515 || id == 546 || id == 516 || id == 486 || id == 517 || id == 487 || id == 457 || id == 488 || id == 458 || id == 489 || id == 459 || id == 490 || id == 460 || id == 399 || id == 429 || id == 398 || id == 428 || id == 397 || id == 427 || id == 396 || id == 426 || id == 456 || id == 425 || id == 455 || id == 485 || id == 454 || id == 484 || id == 453 || id == 483 || id == 513 || id == 482 || id == 512 || id == 481 || id == 511 || id == 480 || id == 510 || id == 479 || id == 509 || id == 539 || id == 569 || id == 538 || id == 568 || id == 598 || id == 567 || id == 506 || id == 537 || id == 507 || id == 477 || id == 508 || id == 478 || id == 448 || id == 418 || id == 449 || id == 419 || id == 450 || id == 420 || id == 451 || id == 421 || id == 452 || id == 422 || id == 392 || id == 423 || id == 393 || id == 424 || id == 394 || id == 364 || id == 395 || id == 365 || id == 335 || id == 366 || id == 336 || id == 367 || id == 337 || id == 368 || id == 338 || id == 283 || id == 310 || id == 282 || id == 309 || id == 281 || id == 308 || id == 280 || id == 307 || id == 334 || id == 306 || id == 333 || id == 363 || id == 332 || id == 362 || id == 331 || id == 361 || id == 391 || id == 360 || id == 390 || id == 359 || id == 389 || id == 358 || id == 388 || id == 357 || id == 387 || id == 417 || id == 447 || id == 416 || id == 446 || id == 476 || id == 445 || id == 415 || id == 384 || id == 354 || id == 385 || id == 355 || id == 386 || id == 356 || id == 326 || id == 299 || id == 327 || id == 300 || id == 328 || id == 301 || id == 329 || id == 302 || id == 330 || id == 303 || id == 276 || id == 304 || id == 277 || id == 305 || id == 278 || id == 256 || id == 279 || id == 257 || id == 238 || id == 258 || id == 239 || id == 259 || id == 240 || id == 260 || id == 241 || id == 261 || id == 311 || id == 369 || id == 430 || id == 491 || id == 522 || id == 583 || id == 644 || id == 704 || id == 753 || id == 805 || id == 863 || id == 922){


			/*
			 
			if (id == 1057 || id == 1770 || id == 1795 || id == 1769 || id == 1818 || id == 1794 || id == 1768 || id == 1708 || id == 1738 || id == 1709 || id == 1739 || id == 1710 || id == 1747 || id == 1749 || id == 1750 || id == 1720 || id == 1690 || id
				== 1630 || id == 1660 || id == 1689 || id == 1719 || id == 1748 || id == 1718 || id == 1688 || id == 1659 || id == 1629 || id == 1601 || id == 1572 || id == 1510 || id == 1541 || id == 1571 || id == 1600 || id == 1628 || id == 1658 || id
				== 1687 || id == 1717 || id == 1746 || id == 1824 || id == 1776 || id == 1823 || id == 1800 || id == 1775 || id == 1822 || id == 1799 || id == 1774 || id == 1821 || id == 1798 || id == 1773 || id == 1820 || id == 1797 || id == 1772 || id
				== 1819 || id == 1796 || id == 1771 || id == 1740 || id == 1711 || id == 1741 || id == 1712 || id == 1742 || id == 1713 || id == 1743 || id == 1714 || id == 1744 || id == 1715 || id == 1745 || id == 1716 || id == 1686 || id == 1657 || id
				== 1627 || id == 1599 || id == 1570 || id == 1540 || id == 1509 || id == 1479 || id == 1448 || id == 1388 || id == 1418 || id == 1447 || id == 1478 || id == 1508 || id == 1539 || id == 1569 || id == 1598 || id == 1626 || id == 1656 || id
				== 1685 || id == 1655 || id == 1684 || id == 1654 || id == 1683 || id == 1653 || id == 1682 || id == 1652 || id == 1681 || id == 1651 || id == 1680 || id == 1650 || id == 1679 || id == 1649 || id == 1678 || id == 1648 || id == 1589 || id
				== 1618 || id == 1590 || id == 1619 || id == 1591 || id == 1620 || id == 1592 || id == 1621 || id == 1593 || id == 1622 || id == 1594 || id == 1623 || id == 1595 || id == 1624 || id == 1596 || id == 1625 || id == 1597 || id == 1568 || id
				== 1506 || id == 1538 || id == 1507 || id == 1477 || id == 1446 || id == 1417 || id == 1387 || id == 1358 || id == 1328 || id == 1299 || id == 1327 || id == 1357 || id == 1386 || id == 1416 || id == 1445 || id == 1476 || id == 1444 || id
				== 1384 || id == 1415 || id == 1385 || id == 1356 || id == 1326 || id == 1298 || id == 1269 || id == 1239 || id == 1178 || id == 1208 || id == 1238 || id == 1268 || id == 1297 || id == 1325 || id == 1355 || id == 1324 || id == 1266 || id
				== 1296 || id == 1267 || id == 1237 || id == 1207 || id == 1177 || id == 1148 || id == 1119 || id == 1060 || id == 1089 || id == 1118 || id == 1147 || id == 1176 || id == 1206 || id == 1236 || id == 1205 || id == 1145 || id == 1175 || id
				== 1146 || id == 1117 || id == 1058 || id == 1088 || id == 1059 || id == 1029 || id == 998 || id == 967 || id == 997 || id == 1028 || id == 996 || id == 1027 || id == 1087 || id == 1116 || id == 1086 || id == 1026 || id == 995 || id == 964 ||
				id == 934 || id == 965 || id == 935 || id == 966 || id == 936 || id == 906 || id == 877 || id == 905 || id == 876 || id == 904 || id == 875 || id == 903 || id == 874 || id == 933 || id == 994 || id == 1056 || id == 1115 || id == 1174 || id
				== 1235 || id == 1295 || id == 1354 || id == 1414 || id == 1475 || id == 1537 || id == 1567 || id == 1536 || id == 1566 || id == 1535 || id == 1565 || id == 1534 || id == 1564 || id == 1533 || id == 1563 || id == 1532 || id == 1562 || id
				== 1531 || id == 1561 || id == 1530 || id == 1560 || id == 1529 || id == 1467 || id == 1498 || id == 1468 || id == 1499 || id == 1469 || id == 1500 || id == 1470 || id == 1501 || id == 1471 || id == 1502 || id == 1472 || id == 1503 || id
				== 1473 || id == 1504 || id == 1474 || id == 1413 || id == 1442 || id == 1412 || id == 1441 || id == 1411 || id == 1440 || id == 1410 || id == 1439 || id == 1409 || id == 1438 || id == 1408 || id == 1437 || id == 1407 || id == 1436 || id
				== 1406 || id == 1346 || id == 1376 || id == 1347 || id == 1377 || id == 1348 || id == 1378 || id == 1349 || id == 1379 || id == 1350 || id == 1380 || id == 1351 || id == 1381 || id == 1352 || id == 1382 || id == 1353 || id == 1294 || id
				== 1322 || id == 1293 || id == 1321 || id == 1292 || id == 1320 || id == 1291 || id == 1319 || id == 1290 || id == 1318 || id == 1289 || id == 1317 || id == 1288 || id == 1316 || id == 1287 || id == 1227 || id == 1258 || id == 1228 || id
				== 1259 || id == 1229 || id == 1260 || id == 1230 || id == 1261 || id == 1231 || id == 1262 || id == 1232 || id == 1263 || id == 1233 || id == 1264 || id == 1234 || id == 1173 || id == 1203 || id == 1172 || id == 1202 || id == 1171 || id
				== 1201 || id == 1170 || id == 1200 || id == 1169 || id == 1199 || id == 1168 || id == 1198 || id == 1167 || id == 1197 || id == 1166 || id == 1107 || id == 1137 || id == 1108 || id == 1138 || id == 1109 || id == 1139 || id == 1110 || id
				== 1140 || id == 1111 || id == 1141 || id == 1112 || id == 1142 || id == 1113 || id == 1143 || id == 1114 || id == 1055 || id == 1084 || id == 1054 || id == 1083 || id == 1053 || id == 1082 || id == 1052 || id == 1081 || id == 1051 || id
				== 1080 || id == 1050 || id == 1079 || id == 1049 || id == 1078 || id == 1048 || id == 986 || id == 1018 || id == 987 || id == 1019 || id == 988 || id == 1020 || id == 989 || id == 1021 || id == 990 || id == 1022 || id == 991 || id == 1023 ||
				id == 992 || id == 1024 || id == 993 || id == 932 || id == 962 || id == 931 || id == 961 || id == 930 || id == 960 || id == 929 || id == 959 || id == 928 || id == 958 || id == 927 || id == 957 || id == 926 || id == 956 || id == 925 || id
				== 866 || id == 895 || id == 867 || id == 896 || id == 868 || id == 897 || id == 869 || id == 898 || id == 870 || id == 899 || id == 871 || id == 900 || id == 872 || id == 901 || id == 873 || id == 902 || id == 963 || id == 1025 || id == 1085 ||
				id == 1144 || id == 1204 || id == 1265 || id == 1323 || id == 1383 || id == 1443 || id == 1505){


				dd.begin(DU_DRAW_POINTS, 4.0f);
				dd.vertex(x, y, z, duRGBA(255, 76, 82, 255));
				dd.end();
			}

			
 
			else {



				dd.begin(DU_DRAW_POINTS, 4.0f);
				dd.vertex(x, y, z, duRGBA(76, 76, 82, 255));
				dd.end();


			}

  

		 
			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(0, 0, 0, duRGBA(255, 76, 82, 255));
			dd.end();

			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(23, 0, 0, duRGBA(12, 255, 82, 255));
			dd.end();

			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(24, 0, 0, duRGBA(255, 26, 82, 255));
			dd.end();


			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(25, 0, 0, duRGBA(12, 25, 255, 255));
			dd.end();

			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(35.5, 0, 0, duRGBA(12, 25, 255, 255));
			dd.end();
			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(35.5, 0, 1, duRGBA(12, 25, 255, 255));
			dd.end();
			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(35.5, 0, -1, duRGBA(12, 25, 255, 255));
			dd.end();

			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(35.5, 0, 2, duRGBA(12, 25, 255, 255));
			dd.end();

			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(35.5, 0, 3, duRGBA(12, 25, 255, 255));
			dd.end();

			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(35.5, 0, 4, duRGBA(12, 25, 255, 255));
			dd.end();


			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(44.5, 0, 5, duRGBA(12, 25, 255, 255));
			dd.end();

			dd.begin(DU_DRAW_POINTS, 9.0f);
			dd.vertex(-50, 0,0 , duRGBA(12, 12, 82, 255));
			dd.end();


			 */

			dd.begin(DU_DRAW_POINTS, 4.0f);
		
			dd.vertex(x, y, z, duRGBA(176, 76, 82, 255));
			dd.vertex(x + size, y, z, duRGBA(176, 76, 82, 255));
			dd.vertex(x - size, y, z, duRGBA(176, 76, 82, 255));

			dd.vertex(x + blueStep, y, z + size, duRGBA(176, 76, 82, 255));
			dd.vertex(x - blueStep, y, z + size, duRGBA(176, 76, 82, 255));

			dd.vertex(x + blueStep, y, z - size, duRGBA(176, 76, 82, 255));
			dd.vertex(x - blueStep, y, z - size, duRGBA(176, 76, 82, 255));
			dd.end();



			//================================== OK hegxacon with line======END===============================



		}



		//=================================================================================================  ok
		/*

		int size = 1;
		float w = 1.5;   // (2 * size) *(3 / 4);  // ==> 1.5   for siz=1   and 4.5 for size =3
		float blueStep = 0.5;  // size / 2;

		int step = size * 3;   // for size=1   ==> step = 3

		for (int z = -10; z <= 10; z+=size){
		for (int x = -10; x <= 10; x+=step){

		//	voronoi v;

		if (z % 2 == 0){


		dd.vertex(x, 0, z, duRGBA(220, 32, 16, 192));

		dd.vertex(x+size, 0, z, duRGBA(0, 0, 255, 192));
		dd.vertex(x - size, 0, z, duRGBA(0, 0, 255, 192));
		dd.vertex(x + blueStep, 0, z + size, duRGBA(0, 0, 255, 192));
		dd.vertex(x - blueStep, 0, z + size, duRGBA(0, 0, 255, 192));
		dd.vertex(x + blueStep, 0, z - size, duRGBA(0, 0, 255, 192));
		dd.vertex(x - blueStep, 0, z - size, duRGBA(0, 0, 255, 192));


		//a.addelement(x,0,z,0);

		}

		else {

		dd.vertex(x + w, 0, z, duRGBA(220, 32, 16, 192));

		//dd.vertex(i + size, 0, z, duRGBA(0, 0, 255, 192));
		//	dd.vertex(i - size, 0, z, duRGBA(0, 0, 255, 192));
		//	dd.vertex(i + 0.25, 0, z + size, duRGBA(0, 0, 255, 192));
		//dd.vertex(i - 0.25, 0, z + size, duRGBA(0, 0, 255, 192));
		//	dd.vertex(i + 0.25, 0, z - size, duRGBA(0, 0, 255, 192));
		//	dd.vertex(i - 0.25, 0, z - size, duRGBA(0, 0, 255, 192));
		// a.addelement(x + w, 0, z, 0);
		}



		}
		}
		*/
		//=================================================================================================  ok

		/*

		dd.end();

		//float x,x2,y2,z2,y,z;
		DebugDrawGL dd;
		dd.begin(DU_DRAW_POINTS, 6.0f);
		//	DebugDrawGL dd;




		//====================================================================================================
		//	GPS_PRINT(x, y, z);
		float x = 0;
		for (int z = -10; z <= 10; z++){
		for (int i = -10; i <= 10; i++){  // X

		if (z % 2 == 0){

		//x = x - i;
		//GPS_PRINT(x, 0, z);
		dd.vertex( i, 0, z, duRGBA(220, 32, 16, 192));
		//duDebugDrawCircle(&dd, x - i, 0, z, 0.5, duRGBA(0, 0, 0, 255), 2.0f);


		}

		else {

		//	x = x - i - 0.5;
		//GPS_PRINT(x, 0, z);
		dd.vertex( i - 0.5, 0, z, duRGBA(220, 32, 16, 192));
		//duDebugDrawCircle(&dd, x - i - 0.5, 0, z, 0.5, duRGBA(0, 0, 0, 255), 2.0f);
		}




		}



		}

		//dd.vertex(0 , 0, 0, duRGBA(0, 0, 255, 255));
		//	dd.end();

		//====================================================================================================





		//float x = 10, y = 0, z = 10;
		//DebugDrawGL dd;


		/*

		dd.begin(DU_DRAW_LINES);
		{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x + 2, 0, z + 1, duRGBA(255, 0, 0, 64));
		dd.vertex(x + 2, 0, z - 1, duRGBA(255, 0, 0, 64));
		dd.end();
		}


		dd.begin(DU_DRAW_LINES);
		{
		//	dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x + 2, 0, z + 1, duRGBA(255, 0, 0, 64));
		dd.vertex(x, 0, z + 2, duRGBA(255, 0, 0, 64));
		}

		dd.begin(DU_DRAW_LINES);
		{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x, 0, z + 2, duRGBA(255, 0, 0, 64));
		dd.vertex(x-2, 0, z + 1, duRGBA(255, 0, 0, 64));
		dd.end();
		}


		dd.begin(DU_DRAW_LINES);
		{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 255));
		dd.vertex(x - 2, 0, z + 1, duRGBA(255, 0, 0, 64));
		dd.vertex(x-2, 0, z -1, duRGBA(255, 0, 0, 64));
		dd.end();
		}


		dd.begin(DU_DRAW_LINES);
		{
		//dd.vertex(x, 0, z, duRGBA(255, 0, 0, 64));
		dd.vertex(x - 2, 0, z - 1, duRGBA(255, 0, 0, 64));
		dd.vertex(x, 0, z - 2, duRGBA(255, 0, 0, 64));
		dd.end();
		}


		dd.begin(DU_DRAW_LINES);
		{
		//dd.vertex(x, 0, z, duRGBA(0, 0, 255, 64));
		dd.vertex(x, 0, z - 2, duRGBA(0, 0, 255, 64));
		dd.vertex(x + 2, 0, z - 1, duRGBA(0, 0, 255, 64));
		dd.end();
		}




		*/

		//dd.end();

		/*

		int id,id2;
		//  std::ifstream in("Random_Points.txt");

		std::ifstream in("v1_Center Of polygons.txt");
		while(in>>id>>x>>y>>z)  {


		dd.vertex(x, y,z, duRGBA(220,32,16,192));
		//40.9848 10.1973 -0.541272


		}

		std::ifstream in2("v2_Inter-edges.txt");
		while(in2>>id2>>x2>>y2>>z2)  {


		dd.vertex(x2, y2,z2, duRGBA(16,32,220,192));

		}

		*/

		// in.close();







	}
	//==========================================


	if (m_nrandPoints3 > 0)
	{



		/*dd.begin(DU_DRAW_POINTS, 6.0f);
		//float x,x2,y2,z2,y,z;da
		float x, y, z;
		int id;
		std::ifstream in("target.txt");
		while (in >> id >> x >> y >> z)
		dd.vertex(x, y, z, duRGBA(220, 32, 16, 192));

		dd.end();

		in.close();
		*/



		//DebugDrawGL dd;
		dd.begin(DU_DRAW_POINTS, 4.0f);
		std::ifstream in5("gpsID5.txt");
		float a, b, c, d, e, f, g;

		while (in5 >> a >> b >> c)
			dd.vertex(a, b, c, duRGBA(255, 76, 82, 255));
		dd.end();
		in5.close();



	}


	//====================================================================================
}

void NavMeshTesterTool::handleRenderOverlay(double* proj, double* model, int* view)
{
	GLdouble x, y, z;

	int i = 0, j = 0;
	float  x2, y2, z2;
	int id, id2;
	char n1;
	int cc;

	//sample->handleRenderOverlay((double*)proj, (double*)model, (int*)view);

	NavMeshTesterTool M;


	M.proj((double*)proj, (double*)model, (int*)view);




	/*



	v2_flag = true;

	//
	std::ifstream in("v1_Center Of polygons2.txt");

	while(in>>id>>x2>>y2>>z2)  {

	char* q= new char[10];
	itoa(id,q,10);
	const char* N = q;

	if (m_sposSet && gluProject((GLdouble)x2, (GLdouble)y2, (GLdouble)z2,
	model, proj, view, &x, &y, &z)  && v2_flag)


	{

	imguiDrawText((int)x, (int)(y), IMGUI_ALIGN_CENTER, N, imguiRGBA(0,0,0,220));
	}

	}




	*/



	// Draw start and end point labels
	if (m_sposSet && gluProject((GLdouble)m_spos[0], (GLdouble)m_spos[1], (GLdouble)m_spos[2],
		model, proj, view, &x, &y, &z))
	{
		imguiDrawText((int)x, (int)(y - 25), IMGUI_ALIGN_CENTER, "Start", imguiRGBA(0, 0, 0, 220));


		//==============================================

		/*

		float x2, y2, z2;
		int poly, id;
		GLdouble xx, yy, zz;
		std::ifstream in("gpsID.txt");
		while (in >> id >> poly >> x2 >> y2 >> z2){


		char* q = new char[10];
		itoa(id, q, 10);
		const char* N = q;

		if ( gluProject((GLdouble)x2, (GLdouble)y2, (GLdouble)z2,	model, proj, view, &xx, &yy, &zz))


		{

		imguiDrawText((int)xx, (int)(yy), IMGUI_ALIGN_CENTER, N, imguiRGBA(76, 76, 82, 255));
		}

		//snprintf(l, 32, "%d", id);
		//	imguiDrawText((int)x, (int)y + 15, IMGUI_ALIGN_CENTER, "*", imguiRGBA(255, 0, 0, 220));
		}

		*/
		//=====================================================================================================








	}
	if (m_eposSet && gluProject((GLdouble)m_epos[0], (GLdouble)m_epos[1], (GLdouble)m_epos[2],
		model, proj, view, &x, &y, &z))
	{
		imguiDrawText((int)x, (int)(y - 25), IMGUI_ALIGN_CENTER, "End", imguiRGBA(0, 0, 0, 220));
	}

	// Tool help
	const int h = view[3];
	imguiDrawText(280, h - 40, IMGUI_ALIGN_LEFT, "LMB+SHIFT: Set start location  LMB: Set end location", imguiRGBA(255, 255, 255, 192));
}

void NavMeshTesterTool::drawAgent(const float* pos, float r, float h, float c, const unsigned int col)
{
	DebugDrawGL dd;

	dd.depthMask(false);

	// Agent dimensions.	
	duDebugDrawCylinderWire(&dd, pos[0] - r, pos[1] + 0.02f, pos[2] - r, pos[0] + r, pos[1] + h, pos[2] + r, col, 2.0f);

	duDebugDrawCircle(&dd, pos[0], pos[1] + c, pos[2], r, duRGBA(0, 0, 0, 64), 1.0f);

	unsigned int colb = duRGBA(0, 0, 0, 196);
	dd.begin(DU_DRAW_LINES);
	dd.vertex(pos[0], pos[1] - c, pos[2], colb);
	dd.vertex(pos[0], pos[1] + c, pos[2], colb);
	dd.vertex(pos[0] - r / 2, pos[1] + 0.02f, pos[2], colb);
	dd.vertex(pos[0] + r / 2, pos[1] + 0.02f, pos[2], colb);
	dd.vertex(pos[0], pos[1] + 0.02f, pos[2] - r / 2, colb);
	dd.vertex(pos[0], pos[1] + 0.02f, pos[2] + r / 2, colb);
	dd.end();

	dd.depthMask(true);
}



void NavMeshTesterTool::proj(double* proj, double* model, int* view){
	//DebugDrawGL dd;
	GLdouble x, y, z;

	if (f == 1){

		float x2, y2, z2;
		int counter, id;
		std::ifstream in("possi.txt");

		std::ofstream fout("col2.txt", std::ios_base::app | std::ios_base::out);

		while (in >> id >> x2 >> y2 >> z2 >> counter){



			//m_navQuery->getPolyHeight(ref, r, &h);


			gluProject((GLdouble)x2, (GLdouble)y2, (GLdouble)z2, model, proj, view, &x, &y, &z);


			//dd.begin(DU_DRAW_POINTS, 9.0f);

			//dd.vertex(x,h, z, duRGBA(176, 76, 182, 255));

			//dd.end();
			
			HDC dc = GetDC(NULL);
			COLORREF col = GetPixel(dc, x, y);
			RGBTRIPLE rgb;

			fout << "\n" << id << " " << col << " " << x2 << "  " << y2 << "  " << z2 << " " << counter;

			//fout << "\n" << x2 << "	" << y2<< "	    " << z2<<"   ||  "<<x << "	" << y << "	    " << z;
		}
		f = 0;

	}
}