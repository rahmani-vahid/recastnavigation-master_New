#include "Navigation.h"
#include "DetourNavMesh.h"
#include "Recast.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include<vector>
#include <stdio.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <time.h>
#include <unordered_map>
#include <thread>
#include <SDL_Thread.h>
#include<ppl.h>
#include<mutex>
#include <agents.h>
#include <windows.h>
//#include <boost/thread.hpp>
#include <omp.h>
#include "PerfTimer.h"
 

 std::mutex mu;

using namespace std;
int co=0;
int v1s;
int v2=0;
int v3;
	// std::multimap<int,  vector<int> > myMap2;
	 unordered_multimap<dtPolyRef, vector<dtPolyRef> > myMap2;
void Navigation::init()
{
	//liberate memory
	for(int i = 0; i < numGraphs; i++)
	{
		graphs[i].nodes->DestroyIntraEdge();
		graphs[i].nodes->DestroyEdge();
		graphs[i].Destroy();			
	}
	
	numGraphs = 0;
	//reserve memory por graphs
	graphs = (Graph*)dtAlloc(sizeof(Graph)*levels, DT_ALLOC_PERM);
	memset(graphs, 0, sizeof(Graph)*levels);
	numLevel = 0;

	//reserve memory por pool nodes
	m_nodePool = 0;
	m_openList = 0;
	if (!m_nodePool || m_nodePool->getMaxNodes() < maxNodes)
	{
		if (m_nodePool)
		{
			m_nodePool->~dtNodePool();
			dtFree(m_nodePool);
			m_nodePool = 0;
		}
		m_nodePool = new (dtAlloc(sizeof(dtNodePool), DT_ALLOC_PERM)) dtNodePool(maxNodes, dtNextPow2(maxNodes/4));
	}
	else
	{
		m_nodePool->clear();
	}

	//reserve memory for open list
	if (!m_openList || m_openList->getCapacity() < maxNodes)
	{
		if (m_openList)
		{
			m_openList->~dtNodeQueue();
			dtFree(m_openList);
			m_openList = 0;
		}
		m_openList = new (dtAlloc(sizeof(dtNodeQueue), DT_ALLOC_PERM)) dtNodeQueue(maxNodes);
	}
	else
	{
		m_openList->clear();
	}

	//get the base poly ref
	refBase = m_navMesh->getPolyRefBase(tile);
	
	 savepath();



}


 


void Navigation::createHierarchicalGraph(int p_levels,int p_level,int p_mergedPolys, rcContext* ctx,const dtMeshTile* ptile,  const dtNavMesh* pm_navMesh, const dtNavMeshQuery* pm_navQuery, std::map<dtPolyRef, int> &nodesInCluster)
{
	rcAssert(ctx);
	
  v1s=p_levels;
	  v3=  p_level;
	if(maxPolyInNode == p_mergedPolys && levels == p_levels)
	{
		level = p_level;
		setGraph();
	}
	else
	{

		ctx->startTimer(RC_TIMER_BUILD_HIERARCHICAL_GRAPH);

		levels = p_levels;
		level = p_level;
		maxPolyInNode = p_mergedPolys;

		tile = ptile;
		m_navMesh = pm_navMesh;
		m_navQuery = pm_navQuery;

		init();

		const dtPoly* poly = 0;
		const dtPoly* neighbourPoly = 0;
		dtPolyRef ref;
		dtPolyRef neighbourRef;
		int numEdgesNode = 0;

		//Main graph
		parentGraph.Init(tile->header->polyCount);
				//create nodes
		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			parentGraph.AddNode(i);
		}
	
		//reserve memory for edges

		for(int i = 0; i < parentGraph.numNodes; i++)
		{
			ref = parentGraph.nodes[i].idNode + refBase;
				
			m_navMesh->getTileAndPolyByRefUnsafe(ref, &tile, &poly);

			for (unsigned int j = poly->firstLink; j != DT_NULL_LINK; j = tile->links[j].next)
			{
				numEdgesNode++;
			}

			if(numEdgesNode > 0)
			{
				parentGraph.InitEdge(ref - refBase, numEdgesNode);
				numEdgesNode = 0;
			}
		}

		//create edges
		int idPos = 1;

		std::map<std::pair<dtPolyRef, dtPolyRef>, float *> positions;

		
		for(int i = 0; i < parentGraph.numNodes; i++)
		{
			//current node
			ref = parentGraph.nodes[i].idNode + refBase;

			m_navMesh->getTileAndPolyByRefUnsafe(ref, &tile, &poly);

			for (unsigned int j = poly->firstLink; j != DT_NULL_LINK; j = tile->links[j].next)
			{
				neighbourRef = tile->links[j].ref;
				m_navMesh->getTileAndPolyByRefUnsafe(neighbourRef, &tile, &neighbourPoly);

				//get position
				float mid[3];
				m_navQuery->getEdgeMidPoint(ref, poly, tile, neighbourRef, neighbourPoly, tile, mid);

				std::pair<dtPolyRef,dtPolyRef> p1;
				std::pair<dtPolyRef,dtPolyRef> p2;
				
				p1 = std::make_pair(ref,neighbourRef);
				auto it1 = positions.find(p1);

				if(it1 == positions.end())
				{
					p2 = std::make_pair(neighbourRef,ref);

					//add edge
					parentGraph.AddEdge(ref - refBase,neighbourRef - refBase, mid, idPos,ref - refBase);
					parentGraph.AddEdge(neighbourRef- refBase,ref - refBase, mid, idPos,neighbourRef- refBase);

					positions.insert( std::pair <      std::pair<dtPolyRef,dtPolyRef>   ,float*>(p1,mid));
					positions.insert(std::pair<std::pair<dtPolyRef,dtPolyRef>,float*>(p2,mid));

					idPos++;
				}
			}
		}
		numTotalEdges = idPos;
		positions.clear();

		//hierarchical subdivisions
		mainGraph = parentGraph;
		graphs[numGraphs++] = parentGraph;
		buildHierarchy();

		setGraph();

		ctx->stopTimer(RC_TIMER_BUILD_HIERARCHICAL_GRAPH);
		 
	 

		//vahid 100
int co=0;
 
		for(int i = 0; i < numGraphs; i++)
		{
			ctx->log(RC_LOG_PROGRESS, ">>Level %d = %d nodes.", i, graphs[i].numNodes);

				
			if(i==v3){
			//fout3 << "\n" <<i<<"  "<<graphs[i].numNodes;


			std::ofstream fout2("v2_Inter-edges.txt", std::ios_base::app | std::ios_base::out);
	
					
			for(int j = 0; j < graphs[i].numNodes; j++)
					{
			

			 
	
		Graph::Node * node =&graphs[i].nodes[j];
		 Graph::Edge *edg=node->edges;

		
		 for(int t=0; t <  node->numEdges;t++){

		 double a=edg[t].pos[0];
		 double a2=edg[t].pos[1];
		 double a3=edg[t].pos[2];
		 co++; 
		 fout2 << "\n" <<node->idNode<<" "<<a <<"   " << a2<<"   " <<a3;
	  }
	 	  				}

	 	} 

		}
//***********************************************************************************

		//}
			
	
	 


//=============================================================================================================
	/*std::ofstream fout2("aaaaa.txt", std::ios_base::app | std::ios_base::out);
		 for(int j = 0; j < graphs[numLevel-1].numNodes; j++)
	{
		
		Graph::Node * node =&graphs[numLevel-1].nodes[j];
			Graph::Edge *edg=node->edges;
		for(int i=0; i<node->numEdges;i++){
			
			fout2 << "\n" <<node->idNode << "     "<<edg[i].pos[0]<<"  "<<edg[i].pos[1]<<"  "<<edg[i].pos[2];
			 
		}
		
//===========================================================
		//<<"  " << node->
			//node[j].idNode;
		//vahid10
	} */

		//vahid19
		std::ofstream fout("Results_old.txt", std::ios_base::app | std::ios_base::out);
		fout << "\n\nMerged Polys: " << p_mergedPolys << "\n";
		for(int i = 0; i < numGraphs; i++)
		{
			fout << "level " << i << ": " <<  graphs[i].numNodes << "\n";
		}
		fout.close();
				}
	
	//get painted nodes

//vahid 101

 


	if(level !=0)
	{
		std::ofstream fout("v3_Cluster.txt", std::ios_base::app | std::ios_base::out);
		for(int i = 0; i < graphs[0].numNodes; i++)
		{
			dtPolyRef nodeId = graphs[0].nodes[i].idNode;
			Graph::Node* node = getNode(nodeId,0);
			
			if(node == NULL){
				nodesInCluster.insert(std::make_pair(nodeId + refBase, -1));
				fout << "\n"<<nodeId + refBase<<"   "<<-1;
							}
			else{
				nodesInCluster.insert(std::make_pair(nodeId + refBase, node->idNode));
			fout << "\n"<<nodeId+refBase<<"   "<<node->idNode;}
		}
		fout.close();


	}
	
 

	//000000000000000000000000000000000000000000000

/*	std::ofstream fout("ver2.txt", std::ios_base::app | std::ios_base::out);
		fout << "\n" << graphs[0].numNodes<<"  ----\n";

		for (int i = 0; i < graphs[0].numNodes; ++i){ // m_npolys
	
				float p0[3];
				dtPolyRef nodeId = graphs[0].nodes[i].idNode + refBase;
				Graph::Node* node = getNode(nodeId,0);
				dtPolyRef ref;
				ref = parentGraph.nodes[i].idNode + refBase;
				dtNavMesh* m_navMesh;

				getPolyCenter(m_navMesh, ref, p0);
	
		fout << "\n" << p0[0]<<" "<<p0[1]<<" "<< p0[2];
		
		}*/


	}

	//============================================================

 

dtStatus Navigation::findPathNav(rcContext* ctx, dtPolyRef startRef, dtPolyRef endRef,const float* startPos, const float* endPos, const dtQueryFilter* filter, dtPolyRef* path, int &pathCount, const int maxPath)
{
	rcAssert(ctx);

	dtPolyRef tempPathNodes[128];
	int tempPathPolys[128];
	int nTempPath = 0;
	float sum=0;
	
	std::ofstream fout("Results_Old.txt", std::ios_base::app | std::ios_base::out);
	//std::ofstream fout9("S_and_G_pos.txt", std::ios_base::app | std::ios_base::out);

	//fout9 << "\n" << startRef << " " << startPos[0] << "  " << startPos[1] << "  " << startPos[2] << "        " << endRef<<"  "<<endPos[0] << "  " << endPos[1] << "  " << endPos[2];
//

	 //std::ofstream fout2("thread.txt", std::ios_base::app | std::ios_base::out);

	ctx->startTimer(RC_TEST2);

	//ctx->startTimer(RC_TIMER_GET_PARENT_NODES);

	Graph::Node *sNode =  getNode(startRef-refBase,0);
	Graph::Node *eNode =  getNode(endRef-refBase,0);

	if(sNode == NULL || eNode==NULL)
		return DT_FAILURE;
	
	//fout << "\n" << maxPolyInNode << " " << level << " " << currentGraph.numNodes<< " "<<sNode->numEdges << " " << eNode->numEdges << " ";
 
 //fout << sNode->numEdges << " " << eNode->numEdges << " ";

	int nSNodeIntraedges = sNode->numIntraEdges;
	int nENodeIntraedges = eNode->numIntraEdges;

	//ctx->stopTimer(RC_TIMER_GET_PARENT_NODES);
	//sum+=RC_TIMER_GET_PARENT_NODES;
	//fout << ctx->getAccumulatedTime(RC_TIMER_GET_PARENT_NODES)/1000.0f << " ";

	 
	 if((level == 0) || (sNode->idNode == eNode->idNode))
	{
	 
		ctx->startTimer(RC_TIMER_FIND_HIERACHICAL_PATH);
		m_navQuery->findPath(startRef, endRef, startPos, endPos, filter, path, &pathCount, MAX_POLYS);
		ctx->stopTimer(RC_TIMER_FIND_HIERACHICAL_PATH);

 //sum+=RC_TIMER_FIND_HIERACHICAL_PATH;
		fout << ctx->getAccumulatedTime(RC_TIMER_FIND_HIERACHICAL_PATH)/1000.0f << " "<<"\n";

		return DT_SUCCESS;
	}
	 
	//fout << "\n" << maxPolyInNode << " " << level << " " << currentGraph.numNodes<< " "<<sNode->numEdges << " " << eNode->numEdges << " ";
 

	 //fout << ctx->getAccumulatedTime(RC_TIMER_GET_PARENT_NODES)/1000.0f << " ";
	//new start and end position id
	int startIdPos = numTotalEdges+1;
	int endIdPos  = startIdPos+1;

	//add node start to graph
	
	//ctx->startTimer(RC_TEST2);

	//test_start(sNode, startRef, startPos, startIdPos);
	//test_end(eNode, endRef, endPos, endIdPos);

	//std::chrono::steady_clock::time_point begin0 = std::chrono::steady_clock::now();
	//ctx->startTimer(RC_TEST2);


	linkStartToGraph(sNode, startRef, startPos, startIdPos);
	 linkEndToGraph(eNode, endRef, endPos, endIdPos);
	 
	// ctx->stopTimer(RC_TEST2);
	// fout << ctx->getAccumulatedTime(RC_TEST2) / 1000.0f << " ";
	 	
	 
	 
	 
	 
	 // fout2 << "\n\Thread = " << std::chrono::duration_cast<std::chrono::milliseconds>(end0 - begin0).count() << "   ";

	/*
	std::chrono::steady_clock::time_point begin0 = std::chrono::steady_clock::now();
	auto t1 = std::chrono::high_resolution_clock::now();
	
	
	test_start(sNode, startRef, startPos, startIdPos);
	 test_end(eNode, endRef, endPos, endIdPos);
	 
	 auto t2 = std::chrono::high_resolution_clock::now();
	  std::chrono::steady_clock::time_point end0 = std::chrono::steady_clock::now();
	 ctx->stopTimer(RC_TEST2);
	
	 std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
	 
	 double d = 0.0f;

	d=std::chrono::duration_cast<std::chrono::milliseconds>(end0 - begin0).count()/1000000;
	 fout2 << "\n\Thread = " <<d  << fp_ms.count()<<"\n";

	 */


//*****************************************************************************************************************
	 /*
	 std::clock_t start;
	 std::clock_t start2;

	 DWORD begin = 0, end = 0;


	 start = std::clock();
	 begin = GetTickCount();

 			  linkStartToGraph(sNode,startRef,startPos,startIdPos);
		      linkEndToGraph(eNode,endRef,endPos, endIdPos);

	 end = GetTickCount();
	 start2 = std::clock();

	 */
	  	   
//*****************************************************************************************************************
	 /*
	 std::clock_t start;
	 std::clock_t start2;

	 DWORD begin = 0, end = 0;

	 
	 start = std::clock();
	 begin = GetTickCount();
	    
	 
	    test_start(sNode,startRef,startPos,startIdPos);  
		test_end(eNode,endRef,endPos, endIdPos);

		end = GetTickCount();
	 start2 = std::clock();

	*/


//==============================================================================
	/*
	std::clock_t start;
	std::clock_t start2;



	start = std::clock();
	ctx->startTimer(RC_TEST2);
	std::chrono::steady_clock::time_point begin0 = std::chrono::steady_clock::now();
	

	                 std::this_thread::sleep_for(std::chrono::seconds(3));


	ctx->stopTimer(RC_TEST2);
	std::chrono::steady_clock::time_point end0 = std::chrono::steady_clock::now();
	start2 = std::clock();



	fout2 << "\n\chrono time = " << std::chrono::duration_cast<std::chrono::seconds>(end0 - begin0).count() << "   ";
	fout2 << "CPU time used: " << (start2 - start) << "   " <<"Recast Timer: "<< ctx->getAccumulatedTime(RC_TEST2)  << "  \n";
	*/

//****************************************boost*************************************************************************
  /*
  std::clock_t start;
    std::clock_t start2;
 


 // start=std::clock();
  ctx->startTimer(RC_TEST2);
  std::chrono::steady_clock::time_point begin0 = std::chrono::steady_clock::now();


  boost::thread t1(&Navigation::linkStartToGraph, this, sNode, startRef, startPos, startIdPos);
  boost::thread t2(&Navigation::linkEndToGraph, this, eNode, endRef, endPos, endIdPos);
  		t1.join();
	   	 t2.join();

 	 std::chrono::steady_clock::time_point end0 = std::chrono::steady_clock::now();
	 ctx->stopTimer(RC_TEST2);

		 fout2 << "\n\Thread = " << std::chrono::duration_cast<std::chrono::milliseconds>(end0 - begin0).count()<<"   " ;
		// fout2 << std::fixed << std::setprecision(2) << "CPU time used: " << (start2 - start) << "   " << ctx->getAccumulatedTime(RC_TEST2) << "  " << ctx->getAccumulatedTime(RC_TEST2)/1000.0f<<"  \n";
	 
				// fout2 << "\n" <<  1000.0* (start2-start)/CLOCKS_PER_SEC;
				  // fout << ctx->getAccumulatedTime(RC_TEST2) / 1000.0f << " ";

		  /// ((double)CLOCKS_PER_SEC ) 1000
		  */	

//*****************************************thread************************************************************************


	  
	 /*

		 std::clock_t start;
    std::clock_t start2;
 


  start=std::clock(); 

   std::thread t3(&Navigation::test_start, this, sNode, startRef, startPos, startIdPos);
	std::thread t2(&Navigation::test_end,this, eNode, endRef, endPos, endIdPos);
	
		t3.join();	 
	 t2.join();
	 ctx->stopTimer(RC_TEST2);
	  start2=std::clock();

				 fout2 << "\n"<< "CPU time used: "  <<  (start2-start) / ((double) CLOCKS_PER_SEC/1000) << " ms\n" ;

		*/		 
	 
	//*****************************************************************************************************************


 
	 /*
	 std::thread t([&]{
	 //mu.lock();
	 test_start(sNode,startRef,startPos,startIdPos);
	 //  mu.unlock();
	 std::thread::id this_id = std::this_thread::get_id();
	 fout2 <<"\n t1: "<< this_id << "\n";

	 });

	 t.join();




	 std::thread t2([&]{
	 //mu.lock();
	 test_end(eNode, endRef, endPos, endIdPos);
	 std::thread::id this_id = std::this_thread::get_id();
	 //  mu.unlock();
	 fout2 <<"\n t2 :"<< this_id << "\n";


	 });


	 t2.join();





	 fout2 << "\nNumber of threads = " << std::thread::hardware_concurrency();


	 */

	 //*****************************************************************************************************************************



	// SDL_Thread t((test_start)(eNode, endRef, endPos, endIdPos), eNode, endRef, endPos, endIdPos);

	// SDL_Thread *SDL_CreateThread(int(*fn)(void *), void *data);

	/// std::thread t2((&test_start), (&eNode, endRef, &endPos, endIdPos));
	//	t2.join();
	 
	
	
	
//Navigation s;

	//thread t1(&Navigation::test_start,&s, sNode, startRef, startPos, startIdPos);
	  
	//	 t1.join();


	// Navigation d;
	// thread t2(&Navigation::test_end,&d, eNode, endRef, endPos, endIdPos);
	// if (t2.joinable())
	//	 t2.join();

	// func1(sNode, startRef, startPos, startIdPos);
	 // func2(eNode, endRef, endPos, endIdPos);
	
 //=================================================================================================
	
	 /*
	 Navigation s;
	 std::thread t1(&Navigation::test_start,&s, std::ref(sNode), std::ref(startRef), std::ref(startPos), std::ref(startIdPos));
	  t1.join();


	  std::thread t2(&Navigation::test_end,&s, std::ref(eNode), std::ref(endRef), std::ref(endPos), std::ref(endIdPos));
	 t2.join();

	 */
	 //=================================================================================================

	// auto thr = std::async(func1, a, b, c, d);
	 // thr.get();

	 //=================================================================================================

	  
 
	// std::thread t1(&Navigation::test_start , &sNode,    startRef ,  &startPos ,  startIdPos );
//t1.join();


	// std::thread t2(&Navigation::test_end , &eNode , endRef, &endPos, endIdPos);
//t2.join();

 
//====================
	 
	
		//fout2 << "\n" << "CPU time used: " << (start2 - start) / ((double)CLOCKS_PER_SEC) << " ms\n";


		//fout2 << "\n" << "clock : " << (start2 - start) <<"    " <<"Tick : " << end - begin << "    "<<"Recact : " << ctx->getAccumulatedTime(RC_TEST2) / 1000.0f;

//		
	 // sum+=RC_TEST2;
   
//	  ctx->stopTimer(RC_TIMER_LINK_START_END_NODES);
	
 

	//find path
	ctx->startTimer(RC_TIMER_FIND_HIERACHICAL_PATH);

	startRef = sNode->idNode;
	endRef = eNode->idNode;

	findHierarchicalPath(startRef, endRef, startIdPos, endIdPos, startPos, endPos, tempPathNodes, tempPathPolys, &nTempPath, maxPath);
	//fout5<<"\n"<<startRef<<" "<< endRef<<" "<<startIdPos<<" "<< endIdPos<<" "<< startPos[0]<<" "<< endPos[0];
	
 

	//ctx->stopTimer(RC_TIMER_FIND_HIERACHICAL_PATH);
	//  //sum+=RC_TIMER_FIND_HIERACHICAL_PATH;
	//fout << ctx->getAccumulatedTime(RC_TIMER_FIND_HIERACHICAL_PATH)/1000.0f << " ";

	int posId;
	int n = 0;

	//start
	//ctx->startTimer(RC_TIMER_GET_SUBPATH);

	if(nTempPath > 1)
	{
		posId = tempPathNodes[1];

		for(unsigned int i = nSNodeIntraedges; i < sNode->numIntraEdges; i++)
		{
			if((sNode->intraEdges[i].startPosId ==  startIdPos) && (sNode->intraEdges[i].endPosId ==  posId))
			{
				memcpy(path, sNode->intraEdges[i].path, sizeof(dtPolyRef)*sNode->intraEdges[i].nPath); 
				n = sNode->intraEdges[i].nPath;
				break;
			}
		}
	}

	//middle
	for(int i = 1; i < nTempPath-1; i++)
	{
		Graph::Node *node = &currentGraph.nodes[tempPathPolys[i]];

		getPath(tempPathNodes[i], tempPathNodes[i+1], node, level, path, n);
	}

	//end
	posId = tempPathNodes[nTempPath-1];

	for(unsigned int i = nENodeIntraedges; i < eNode->numIntraEdges; i++)
	{
		if((eNode->intraEdges[i].startPosId ==  posId) && (eNode->intraEdges[i].endPosId ==  endIdPos))
		{
			memcpy(path + n, eNode->intraEdges[i].path, sizeof(dtPolyRef)*eNode->intraEdges[i].nPath);
			n += eNode->intraEdges[i].nPath;
			break;
		}
	}

	//ctx->stopTimer(RC_TIMER_GET_SUBPATH);
	// sum+=RC_TIMER_GET_SUBPATH;
//	fout << ctx->getAccumulatedTime(RC_TIMER_GET_SUBPATH)/1000.0f << " ";

	pathCount = n;

	//delete start node and links
	//ctx->startTimer(RC_TIMER_DELETE_START_END_NODES);

	sNode->numIntraEdges = nSNodeIntraedges;
	eNode->numIntraEdges = nENodeIntraedges;
		
	//ctx->stopTimer(RC_TIMER_DELETE_START_END_NODES);
	// sum+=RC_TIMER_DELETE_START_END_NODES;
	//fout << ctx->getAccumulatedTime(RC_TIMER_DELETE_START_END_NODES)/1000.0f<<" ";
	//fout<<sum;
	//fout.close();




	ctx->stopTimer(RC_TEST2);
	fout << ctx->getAccumulatedTime(RC_TEST2) / 1000.0f << " ";





	return DT_SUCCESS;






}

/*

dtStatus Navigation::findPathNav2(rcContext* ctx, dtPolyRef startRef, dtPolyRef endRef,const float* startPos, const float* endPos, const dtQueryFilter* filter, dtPolyRef* path, int &pathCount, const int maxPath)
{
	rcAssert(ctx);
	
	std::ofstream fout("Results_New.txt", std::ios_base::app | std::ios_base::out);
	
	

	dtPolyRef tempPathNodes[128];
	int tempPathPolys[128];
	int nTempPath = 0;

	ctx->startTimer(RC_TIMER_GET_PARENT_NODES_NEW);

	Graph::Node *sNode =  getNode(startRef-refBase,0);
	Graph::Node *eNode =  getNode(endRef-refBase,0);

	if(sNode == NULL || eNode==NULL)
		return DT_FAILURE;
	ctx->stopTimer(RC_TIMER_GET_PARENT_NODES_NEW);


	if(sNode->idNode != eNode->idNode){
		fout << "\n" << maxPolyInNode << " " << level << " " << currentGraph.numNodes << " ";  //================
	fout << sNode->numEdges << " " << eNode->numEdges << " ";//================
	}

	int nSNodeIntraedges = sNode->numIntraEdges;
	int nENodeIntraedges = eNode->numIntraEdges;

	//fout.close();
	//std::ofstream fout2("info.txt", std::ios_base::app | std::ios_base::out);
//	fout2 << "\n" << "sNode->numEdges = " << "  " << sNode->numEdges << "         " << "eNode->numEdges = " << "  "<<eNode->numEdges<<"  ";
	//fout2 << "nSNodeIntraedges = " << " " << nSNodeIntraedges <<"       "<< "nENodeIntraedges = " << " " << nENodeIntraedges;
	//fout2.close();

	if(sNode->idNode != eNode->idNode)
	//std::ofstream fout("results2.txt", std::ios_base::app | std::ios_base::out);
	fout << ctx->getAccumulatedTime(RC_TIMER_GET_PARENT_NODES_NEW)/1000.0f << " ";

	
	//******************************************vahid1****************************
	/////////if (level !=0 ){
	///////////	m_navQuery->findPath(startRef, endRef, startPos, endPos, filter, path, &pathCount, MAX_POLYS);
			
		/////////}
//_____________________________________________________________________________________________________________________________________________________
	if((level == 0) || (sNode->idNode == eNode->idNode))
	{
		ctx->startTimer(RC_TIMER_FIND_HIERACHICAL_PATH_NEW);
		
		//***********check**************
		m_navQuery->findPath(startRef, endRef, startPos, endPos, filter, path, &pathCount, MAX_POLYS);
		ctx->stopTimer(RC_TIMER_FIND_HIERACHICAL_PATH_NEW);

		fout << ctx->getAccumulatedTime(RC_TIMER_FIND_HIERACHICAL_PATH_NEW)/1000.0f << " ";

		return DT_SUCCESS;
	}
	//_____________________________________________________________________________________________________________________________________________________
	//new start and end position id
	int startIdPos = numTotalEdges+1;
	int endIdPos  = startIdPos+1;


	//add node start to graph
	ctx->startTimer(RC_TIMER_LINK_START_END_NODES_NEW);
//			linkStartToGraph2(sNode,startRef,startPos,startIdPos);
			linkEndToGraph2(eNode,endRef,endPos, endIdPos);
	ctx->stopTimer(RC_TIMER_LINK_START_END_NODES_NEW);
	
	
	if(sNode->idNode != eNode->idNode)
			fout <<" | "<< ctx->getAccumulatedTime(RC_TIMER_LINK_START_END_NODES_NEW)/1000.0f << " ";


	//ctx->startTimer(RC_TEST);
	
	//ctx->stopTimer(RC_TEST);

	//fout <<ctx->getAccumulatedTime(RC_TEST)/1000.0f << " | ";

	
	//find path
	ctx->startTimer(RC_TIMER_FIND_HIERACHICAL_PATH_NEW);

	startRef = sNode->idNode;
	endRef = eNode->idNode;
	findHierarchicalPath(startRef, endRef, startIdPos, endIdPos, startPos, endPos, tempPathNodes, tempPathPolys, &nTempPath, maxPath);

	ctx->stopTimer(RC_TIMER_FIND_HIERACHICAL_PATH_NEW);
	if(sNode->idNode != eNode->idNode)
	fout <<ctx->getAccumulatedTime(RC_TIMER_FIND_HIERACHICAL_PATH_NEW)/1000.0f << " ";

	int posId;
	int n = 0;

	//start
	ctx->startTimer(RC_TIMER_GET_SUBPATH_NEW);

	if(nTempPath > 1)
	{
		posId = tempPathNodes[1];

		for(unsigned int i = nSNodeIntraedges; i < sNode->numIntraEdges; i++)
		{
			if((sNode->intraEdges[i].startPosId ==  startIdPos) && (sNode->intraEdges[i].endPosId ==  posId))
			{
				memcpy(path, sNode->intraEdges[i].path, sizeof(dtPolyRef)*sNode->intraEdges[i].nPath); 
				n = sNode->intraEdges[i].nPath;
				break;
			}
		}
	}

	//middle
	for(int i = 1; i < nTempPath-1; i++)
	{
		Graph::Node *node = &currentGraph.nodes[tempPathPolys[i]];

		getPath(tempPathNodes[i], tempPathNodes[i+1], node, level, path, n);
	}

	//end
	posId = tempPathNodes[nTempPath-1];

	for(unsigned int i = nENodeIntraedges; i < eNode->numIntraEdges; i++)
	{
		if((eNode->intraEdges[i].startPosId ==  posId) && (eNode->intraEdges[i].endPosId ==  endIdPos))
		{
			memcpy(path + n, eNode->intraEdges[i].path, sizeof(dtPolyRef)*eNode->intraEdges[i].nPath);
			n += eNode->intraEdges[i].nPath;
			break;
		}
	}

	ctx->stopTimer(RC_TIMER_GET_SUBPATH_NEW);
	if(sNode->idNode != eNode->idNode)
	fout <<   ctx->getAccumulatedTime(RC_TIMER_GET_SUBPATH_NEW)/1000.0f << " ";

	pathCount = n;

	//delete start node and links
	ctx->startTimer(RC_TIMER_DELETE_START_END_NODES_NEW);

	sNode->numIntraEdges = nSNodeIntraedges;
	eNode->numIntraEdges = nENodeIntraedges;
		
	ctx->stopTimer(RC_TIMER_DELETE_START_END_NODES_NEW);
	if(sNode->idNode != eNode->idNode)
	fout  <<ctx->getAccumulatedTime(RC_TIMER_DELETE_START_END_NODES_NEW)/1000.0f<<" " ;
	
	fout.close();
	
	return DT_SUCCESS;
}

*/
void Navigation::getPath(int fromPosId, int toPosId, Graph::Node *node, int l, dtPolyRef* tempPath, int &nTempPath)
{
	
	
	if(l==0)
		return;

	for(unsigned int i = 0; i < node->numIntraEdges; i++)
	{
		if((node->intraEdges[i].startPosId ==  fromPosId) && (node->intraEdges[i].endPosId ==  toPosId))
		{
			Graph::IntraEdge intraEdge =node->intraEdges[i];

			l--;
			if(l==0)
			{
				memcpy(tempPath + nTempPath, intraEdge.path, sizeof(dtPolyRef)*intraEdge.nPath);
				nTempPath += intraEdge.nPath;
			
				return;
			}
			else
			{
				dtPolyRef nextRef;
				int startIdPos;
				int endIdPos;

				startIdPos = fromPosId;
		
				//for each subNode
				int n = intraEdge.nPath;
				for(int j = 0; j < n; j++ )  
				{
					Graph::Node * subNode = &graphs[l].nodes[intraEdge.path[j]];
					nextRef = intraEdge.path[(j+1)%n];

					if(j==n-1)
					{
						endIdPos = toPosId;
					}
					else
					{
						unsigned int numSubEdges =  subNode->numEdges;

						for(unsigned int k = 0; k < numSubEdges; k++ )  
						{
							if(subNode->edges[k].targetNodeId == nextRef)
							{
								endIdPos = subNode->edges[k].idPos;
								break;
							}
						}
					}

					getPath(startIdPos, endIdPos, subNode, l,tempPath, nTempPath);

					startIdPos = endIdPos;
				}

			}
			break;
		}
		
	}
}

dtStatus Navigation::findHierarchicalPath(dtPolyRef startRef, dtPolyRef endRef, int  startIdPos, int endIdPos, const float* startPos, const float* endPos,  dtPolyRef* tempPathNodes, int* tempPathPolys, int *nTempPath, const int maxPath)
{
	std::ofstream fout("ASTAR.txt", std::ios_base::app | std::ios_base::out);


	const TimeVal startTime = getPerfTime();

 

	dtAssert(m_nodePool);
	dtAssert(m_openList);

	m_nodePool->clear();
	m_openList->clear();

	dtNode* startNode = m_nodePool->getNode(startIdPos);

	dtVcopy(startNode->pos, startPos);
	startNode->pidx = 0;
	startNode->cost = 0;
	startNode->total = dtVdist(startPos, endPos) * H_SCALE;
	startNode->id = startIdPos;
	startNode->idPos = startRef;
	startNode->flags = DT_NODE_OPEN;
	m_openList->push(startNode);
	
	dtNode* lastBestNode = startNode;
	float lastBestNodeCost = startNode->total;

	dtStatus status = DT_SUCCESS;

	const TimeVal endTime = getPerfTime();
	int deltaTime = (int)(endTime - startTime);
	fout << "\n" << deltaTime / 1000.0f << "  ";

   





	const TimeVal startTime2 = getPerfTime();

	while (!m_openList->empty())
	{
		// Remove node from open list and put it in closed list.
		dtNode* bestNode = m_openList->pop();
		bestNode->flags &= ~DT_NODE_OPEN;
		bestNode->flags |= DT_NODE_CLOSED;
		
		// Reached the goal, stop searching.
		if (bestNode->id == endIdPos)
		{
			lastBestNode = bestNode;
			break;
		}

		const Graph::Node* node = &currentGraph.nodes[bestNode->idPos];

		// Get parent
		dtPolyRef parentRef = -1;

		if (bestNode->pidx)
			parentRef = m_nodePool->getNodeAtIdx(bestNode->pidx)->id;

		for(unsigned int i=0; i < node->numEdges; i++)
		{
			dtPolyRef neighbourRef = node->edges[i].idPos;

			if(bestNode->id ==neighbourRef)
				continue;

			if (neighbourRef == parentRef)
				continue;

			dtNode* neighbourNode = m_nodePool->getNode(neighbourRef);

			if(!neighbourNode)
				continue;

			// If the node is visited the first time, calculate node position.
			if (neighbourNode->flags == 0)
			{
				dtVcopy(neighbourNode->pos,  node->edges[i].pos);
			}

			// Calculate cost and heuristic.
			float cost = 0.0f;
			float heuristic = 0.0f;

			// Special case for last node.
			if (neighbourRef == endIdPos)
			{
				const float curCost = getCost(node, bestNode->id, neighbourNode->id, bestNode->pos, neighbourNode->pos);
				//const Graph::Node* eNode = currentGraph.nodes[endRef];
				//const float endCost = getCost(eNode, neighbourNode->id, endIdPos, neighbourNode->pos, endPos);
				const float endCost = getCost(node, neighbourNode->id, endIdPos, neighbourNode->pos, endPos);

				cost = bestNode->cost + curCost + endCost;
				heuristic = 0.0f;
			}
			else
			{

				const float curCost = getCost(node, bestNode->id, neighbourNode->id,bestNode->pos, neighbourNode->pos);
				
				cost = bestNode->cost + curCost;
				heuristic = dtVdist(neighbourNode->pos, endPos)*H_SCALE;
			}

			const float total = cost + heuristic;
			
			// The node is already in open list and the new result is worse, skip.
			if ((neighbourNode->flags & DT_NODE_OPEN) && total >= neighbourNode->total)
				continue;
			// The node is already visited and process, and the new result is worse, skip.
			if ((neighbourNode->flags & DT_NODE_CLOSED) && total >= neighbourNode->total)
				continue;
			
			// Add or update the node.
			neighbourNode->pidx = m_nodePool->getNodeIdx(bestNode);
			neighbourNode->idPos = node->edges[i].targetNodeId;
			neighbourNode->id = neighbourRef;
			neighbourNode->flags = (neighbourNode->flags & ~DT_NODE_CLOSED);
			neighbourNode->cost = cost;
			neighbourNode->total = total;
			







			if (neighbourNode->flags & DT_NODE_OPEN)
			{
				// Already in open, update node location.
				m_openList->modify(neighbourNode);
			}
			else
			{
				// Put the node in open list.
				neighbourNode->flags |= DT_NODE_OPEN;
				m_openList->push(neighbourNode);
			}
			
			// Update nearest node to target so far.
			if (heuristic < lastBestNodeCost)
			{
				lastBestNodeCost = heuristic;
				lastBestNode = neighbourNode;
			}		
		}

	
	}
	



	const TimeVal endTime2 = getPerfTime();
	int deltaTime2 = (int)(endTime2 - startTime2);
	fout << "\n" << deltaTime2 / 1000.0f << "  ";


	//const TimeVal startTime3 = getPerfTime();


	// Reverse the path.
	dtNode* prev = 0;
	dtNode* node = lastBestNode;
	do
	{
		dtNode* next = m_nodePool->getNodeAtIdx(node->pidx);
		node->pidx = m_nodePool->getNodeIdx(prev);
		prev = node;
		node = next;
	}
	while (node);
	
	// Store path

	node = prev;
	int n = 0;
	do
	{
	
		tempPathNodes[n] = node->id;
		tempPathPolys[n] = node->idPos;
		n++;

		if (n >= maxPath)
		{
			break;
		}
		node = m_nodePool->getNodeAtIdx(node->pidx);
	}
	while (node);
	
	*nTempPath = n;
	return status;


	//const TimeVal endTime3 = getPerfTime();
//	int deltaTime3 = (int)(endTime3 - startTime3);
//	fout << "\n" << deltaTime3 / 1000.0f << "  ";
//
}

 
void Navigation::linkStartToGraph(Graph::Node *node, dtPolyRef startRef, const float *startPos, int startIdPos)
{
	//omp_lock_t writelock;
	//omp_init_lock(&writelock);
//#pragma omp parallel 
 	for(unsigned int i=0; i < node->numEdges; i++)
	{
		dtPolyRef m_polys[MAX_POLYS];
		int m_npolys;
		//omp_set_lock(&writelock);
		float cost = findPath(startRef - refBase, node->edges[i].idPoly, startPos, node->edges[i].pos,m_polys,m_npolys,MAX_POLYS);
		currentGraph.AddIntraEdge(node->idNode, startIdPos, node->edges[i].idPos, cost, m_polys,m_npolys);
		//omp_unset_lock(&writelock);
	} 
}


void Navigation::linkEndToGraph(Graph::Node *node, dtPolyRef startRef, const float *startPos, int startIdPos)
{
	//omp_lock_t writelock;
	//omp_init_lock(&writelock);

//#pragma omp parallel 
	for(unsigned int i=0; i < node->numEdges; i++)
	{
		dtPolyRef m_polys[MAX_POLYS];
		int m_npolys;	
		//omp_set_lock(&writelock);
		float cost = findPath(node->edges[i].idPoly, startRef - refBase, node->edges[i].pos, startPos,m_polys,m_npolys,MAX_POLYS);

		currentGraph.AddIntraEdge(node->idNode, node->edges[i].idPos, startIdPos, cost, m_polys,m_npolys);
		//omp_unset_lock(&writelock);
	}
}


 
float Navigation::getCost(const Graph::Node *node, int startPosId, int endPosId, const float * startPos, const float * endPos)
{
	if(startPosId == endPosId )
		return 0.0f;

	//------
	//vahid_104

	/*for(unsigned int i = 0; i < node->numIntraEdges; i++)
	{
		if((node->intraEdges[i].startPosId ==  startPosId) && (node->intraEdges[i].endPosId ==  endPosId))
		{
			return node->intraEdges[i].cost;
		}
	}*/
	
	//------

	return dtVdist(startPos, endPos);
}

void Navigation::buildHierarchy()
{
	do{
		//merge nodes

		//numParts =	floor((float(parentGraph.numNodes) / float(maxPolyInNode)) +0.5f);
		numParts = 	parentGraph.numNodes / maxPolyInNode;

		if(numParts < 2 || numParts >= parentGraph.numNodes)
			break;

		//merge nodes and build cluster
		mergeNodes();

		//create subgraph
		buildNodes();
		buildEdges();

		graphs[numGraphs++] = currentGraph;
		
		parentGraph.nodes = currentGraph.nodes;
		parentGraph.numNodes = currentGraph.numNodes;

		nodeCluster.clear();
		clusterNode.clear();
		
		numLevel++;
		//00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

	}while(numLevel <= levels);





 /*	std::ofstream fout2("v2_Inter-edges.txt", std::ios_base::app | std::ios_base::out);
		//	std::ofstream fout3("lev.txt", std::ios_base::app | std::ios_base::out);
		//	fout3 << "\n" << "   "<<" "<<numGraphs<<"  "<<graphs[numGraphs].numNodes<<"  "<<levels<<"   "<<numLevel<<"   "<<graphs[numLevel].numNodes;
			 
	  for(int j = 0; j < graphs[2].numNodes; j++)
	{
	
		
		Graph::Node * node =&graphs[2].nodes[j];
		Graph::Edge *edg=node->edges;
		for(int i=0; i <= node->numEdges;i++){
			
			fout2 << "\n" <<node->idNode << "     "<<edg[i].pos[0]<<"  "<<edg[i].pos[1]<<"  "<<edg[i].pos[2];
			 
		}

		//vahid10
	} 
 
	 */



}



void Navigation::mergeNodes()
{
	int maxLinkCount = 0;
	
	//number edges
	for(int j = 0; j < parentGraph.numNodes; j++)
	{
		Graph::Node * node = &parentGraph.nodes[j];
		for(unsigned int i=0; i < node->numEdges; i++)
		{
			
			maxLinkCount++;
		}
	}

	idxtype* xadj = (idxtype*)dtAlloc( (parentGraph.numNodes+1)*sizeof(idxtype), DT_ALLOC_TEMP );
	idxtype* adjncy = (idxtype*)dtAlloc( maxLinkCount*2*sizeof(idxtype), DT_ALLOC_TEMP );
	idxtype* part = (idxtype*)dtAlloc( parentGraph.numNodes * sizeof(idxtype), DT_ALLOC_TEMP );

	//get parameters
	int xID = 0;
	int adjID = 0;
	unsigned int ip;

	for(int j = 0; j < parentGraph.numNodes; j++)
	{
		xadj[xID++] = adjID;
		Graph::Node * node = &parentGraph.nodes[j];
		for(unsigned int i=0; i < node->numEdges; i++)
		{
			ip = node->edges[i].targetNodeId;
			adjncy[adjID++] = ip;
		}
	}	
	xadj[xID] = adjID;

	idxtype wgtflag = 0;
	idxtype numflag = 0;
	int options[5] = {0,0,0,0,0};
	idxtype edgecut = -1;

	//Metis
	METIS_PartGraphKway(&parentGraph.numNodes, xadj, adjncy, NULL, NULL, &wgtflag, &numflag, &numParts, options, &edgecut, part);

	//get partitions (check merged nodes are neighbours
	checkPartition(part, parentGraph.numNodes, numParts);


	
	//set the partition number for each node
	int partCount = 0;

	for(int j = 0; j < parentGraph.numNodes; ++j)
	{
		int idC = part[partCount++];

		Graph::Node *node = &parentGraph.nodes[j];

		if(!node)
			continue;

		if(node->numEdges <= 0)
			continue;

		clusterNode.insert(std::pair<dtPolyRef, dtPolyRef>(idC, j));
		//vahid12

		
	//std::ofstream fout3("ppppp.txt", std::ios_base::app | std::ios_base::out);
	//fout3 << "\n" << j <<  "    " << idC;
	
	}

	//add parent for each child
	int nodeId = 0;

	for(auto it = clusterNode.begin(), end = clusterNode.end(); it != end; it = clusterNode.upper_bound(it->first))
	{
		auto ret = clusterNode.equal_range(it->first);
		for (auto it1=ret.first; it1!=ret.second; ++it1)
		{
			nodeCluster.insert(std::pair<dtPolyRef, dtPolyRef>		(it1->second, nodeId));
			parentGraph.AddParent(it1->second, nodeId);
		}
		nodeId++;
	}

	//create memory for nodes in graph
	clusterNode.clear();
	currentGraph.Init(nodeId);

	for(auto it = nodeCluster.begin(); it != nodeCluster.end(); ++it ) 
	{
		clusterNode.insert(std::pair<dtPolyRef, dtPolyRef>(it->second, it->first));	
	}
			
	


	dtFree(xadj);
	dtFree(adjncy);
	dtFree(part);
}

void Navigation::buildNodes()
{
	//nodes
	for(auto it = clusterNode.begin(), end = clusterNode.end(); it != end; it = clusterNode.upper_bound(it->first))
	{
		currentGraph.AddNode(it->first);
	}
}

void Navigation::buildEdges()
{
	std::multimap<std::pair<dtPolyRef,dtPolyRef>, std::pair<int ,float*>> portals;
	std::vector<dtPolyRef> subgraphNodes;

	dtPolyRef clusterId = 0;
	dtPolyRef neighbourClusterId = 0;

	
	// create memory for edges
	int numEdgesNode = 0;
	for(auto it = clusterNode.begin(), end = clusterNode.end(); it != end; it = clusterNode.upper_bound(it->first))
	{
		clusterId = it->first;
		
		


		auto ret = clusterNode.equal_range(clusterId);
		for (auto it1=ret.first; it1!=ret.second; ++it1)
		{
			Graph::Node * node = &parentGraph.nodes[it1->second];
			for(unsigned int i=0; i < node->numEdges; i++)
			{
				Graph::Node *neighbour =  &parentGraph.nodes[node->edges[i].targetNodeId];
				neighbourClusterId = nodeCluster.find(neighbour->idNode)->second;
			



				if(neighbourClusterId == clusterId)
					continue;

				Graph::Node *tempNode = &graphs[numLevel].nodes[node->idNode];

			
				


				for(unsigned int j=0; j < tempNode->numEdges; j++)
				{
					if(tempNode->edges[j].idPos == node->edges[i].idPos)
					{
						numEdgesNode++;
					}
				}
			}			
		}

		if(numEdgesNode > 0)
		{
			currentGraph.InitEdge(clusterId, numEdgesNode);
			numEdgesNode = 0;
		}
	}

	
	//create edges
	for(auto it = clusterNode.begin(), end = clusterNode.end(); it != end; it = clusterNode.upper_bound(it->first))
	{
		clusterId = it->first;

		//group of nodes
		auto ret = clusterNode.equal_range(clusterId);

		//for each node in the cluster
		for (auto it1=ret.first; it1!=ret.second; ++it1)
		{
			Graph::Node * node = &parentGraph.nodes[it1->second];
			//node->idNode

			//detect portal nodes between two clusters
			for(unsigned int i=0; i < node->numEdges; i++)
			{
				Graph::Node *neighbour =  &parentGraph.nodes[node->edges[i].targetNodeId];
				neighbourClusterId = nodeCluster.find(neighbour->idNode)->second;

				//add subgraph nodes of thecluster
				if(std::find(subgraphNodes.begin(), subgraphNodes.end(), node->edges[i].idPos)==subgraphNodes.end())
				{
					subgraphNodes.push_back(node->edges[i].idPos);
				}
			
				if(neighbourClusterId == clusterId)
					continue;

				//add edge
				Graph::Node *tempNode = &graphs[numLevel].nodes[node->idNode];

				for(unsigned int j=0; j < tempNode->numEdges; j++)
				{
					if(tempNode->edges[j].idPos == node->edges[i].idPos)
					{
						currentGraph.AddEdge(clusterId,neighbourClusterId, node->edges[i].pos, node->edges[i].idPos,tempNode->edges[i].idPoly);
						break;
					}
				}

				//portals
				std::pair<dtPolyRef,dtPolyRef> p = std::make_pair(node->idNode, neighbour->idNode);
				std::pair<int,float*> p1 = std::make_pair(node->edges[i].idPos, node->edges[i].pos);
				portals.insert(std::make_pair(p, p1));		
			}			
		}

		Graph::Node *currentNode = &currentGraph.nodes[clusterId];
		currentNode->InitIntraEdge();

		//add intraedges
		for(auto it1 = portals.begin(); it1 != portals.end(); ++it1 ) 
		{
			std::pair<dtPolyRef,dtPolyRef> p = it1->first;

			dtPolyRef idNode = p.first;

			std::pair<int,float *> p1 = it1->second;

			float nodePos[3];
			int idPos = p1.first;
			dtVcopy(nodePos, p1.second);

			//intraedges
			for(auto it2 = portals.begin(); it2 != portals.end(); ++it2 ) 
			{
				if(it1==it2)
					continue;

				std::pair<dtPolyRef,dtPolyRef> p2 = it2->first;
				dtPolyRef idNeighbour = p2.first;
			
				float neighbourPos[3];
				std::pair<int,float*> p3 = it2->second;
				int neiughbourIdPos = p3.first;
				dtVcopy(neighbourPos, p3.second);
				dtPolyRef m_polys[MAX_POLYS];
				int m_npolys;

				float cost = findPathLocal(idNode, idNeighbour, idPos, neiughbourIdPos, nodePos,neighbourPos,m_polys,m_npolys,MAX_POLYS,subgraphNodes);
				
				currentGraph.AddIntraEdge(clusterId, idPos, neiughbourIdPos, cost, m_polys, m_npolys);
			}
		}

		subgraphNodes.clear();
		portals.clear();
	}
}



 float Navigation::findPath(dtPolyRef startRef, dtPolyRef endRef,const float* startPos, const float* endPos, dtPolyRef* path, int &pathCount, const int maxPath)
{
	float cost = 0.0f;
	dtAssert(m_nodePool);
	dtAssert(m_openList);

	//dtAssert(m_nodePool_2);
	//dtAssert(m_openList_2);
	
	pathCount = 0;
	
	m_nodePool->clear();
	m_openList->clear();
	
	dtNode* startNode = m_nodePool->getNode(startRef);
	dtVcopy(startNode->pos, startPos);
	startNode->pidx = 0;
	startNode->cost = 0;
	startNode->total = dtVdist(startPos, endPos) * H_SCALE;
	startNode->id = startRef;
	startNode->flags = DT_NODE_OPEN;
	m_openList->push(startNode);
	dtNode* lastBestNode = startNode;
	float lastBestNodeCost = startNode->total;
	
	while (!m_openList->empty())
	{

		// Remove node from open list and put it in closed list.
		dtNode* bestNode = m_openList->pop();
		bestNode->flags &= ~DT_NODE_OPEN;
		bestNode->flags |= DT_NODE_CLOSED;
		
		// Reached the goal, stop searching.
		if (bestNode->id == endRef)
		{
			lastBestNode = bestNode;
			break;
		}
		
		Graph::Node* node = &mainGraph.nodes[bestNode->id];
		
		// Get parent poly and tile.
		dtPolyRef parentRef = 0;
	
		if (bestNode->pidx)
			parentRef = m_nodePool->getNodeAtIdx(bestNode->pidx)->id;

		
//==================================================================================
//#pragma omp parallel 
		for (unsigned int i=0; i < node->numEdges; i++)
		{
			dtPolyRef neighbourRef = node->edges[i].targetNodeId;

			// Skip invalid ids and do not expand back to where we came from.
			if (!neighbourRef || neighbourRef == parentRef)
				continue;

			dtNode* neighbourNode = m_nodePool->getNode(neighbourRef);
			if (!neighbourNode)
			{
				continue;
			}
			
			// If the node is visited the first time, calculate node position.
			if (neighbourNode->flags == 0)
			{
				dtVcopy(neighbourNode->pos,  node->edges[i].pos);
			}

			// Calculate cost and heuristic.
			float cost = 0.0f;
			float heuristic = 0.0f;
			
			if (neighbourRef == endRef)
			{
				const float curCost = dtVdist(bestNode->pos, neighbourNode->pos);
				const float endCost = dtVdist(bestNode->pos, endPos);

				cost = bestNode->cost + curCost + endCost;
				heuristic = 0.0f;
			}
			else
			{
				const float curCost = dtVdist(bestNode->pos, neighbourNode->pos);
				cost = bestNode->cost + curCost;
				heuristic = dtVdist(neighbourNode->pos, endPos)*H_SCALE;
			}
						const float total = cost + heuristic;
			
			// The node is already in open list and the new result is worse, skip.
			if ((neighbourNode->flags & DT_NODE_OPEN) && total >= neighbourNode->total)
				continue;
			// The node is already visited and process, and the new result is worse, skip.
			if ((neighbourNode->flags & DT_NODE_CLOSED) && total >= neighbourNode->total)
				continue;
			
			// Add or update the node.
			neighbourNode->pidx = m_nodePool->getNodeIdx(bestNode);
			neighbourNode->id = neighbourRef;
			neighbourNode->flags = (neighbourNode->flags & ~DT_NODE_CLOSED);
			neighbourNode->cost = cost;
			neighbourNode->total = total;
			
			if (neighbourNode->flags & DT_NODE_OPEN)
			{
				// Already in open, update node location.
				m_openList->modify(neighbourNode);
			}
			else
			{
				// Put the node in open list.
				neighbourNode->flags |= DT_NODE_OPEN;
				m_openList->push(neighbourNode);
			
			// vahid_1120_connect start to the graph===> chaech estimated distance 
			
			}
			
			// Update nearest node to target so far.
			if (heuristic < lastBestNodeCost)
			{
				lastBestNodeCost = heuristic;
				lastBestNode = neighbourNode;
			}
		}

//==================================================================================

	}
		
	// Reverse the path.
	dtNode* prev = 0;
	dtNode* node = lastBestNode;
	cost = lastBestNode->cost;
	do
	{
		dtNode* next = m_nodePool->getNodeAtIdx(node->pidx);
		node->pidx = m_nodePool->getNodeIdx(prev);
		prev = node;
		node = next;
	}
	while (node);
	
	// Store path
	node = prev;
	int n = 0;
	do
	{
		path[n++] = node->id + refBase;
		if (n >= maxPath)
		{
			break;
		}
		node = m_nodePool->getNodeAtIdx(node->pidx);
	}
	while (node);
	
	pathCount = n;
	
	return cost;
		/*std::ofstream fout3("path.txt", std::ios_base::app | std::ios_base::out);
	for(int i=0;i<n;i++){
	
			fout3 << "\n" <<cost;

	}*/
	}


float Navigation::findPathLocal(dtPolyRef startRef, dtPolyRef endRef,int  startIdPos, int endIdPos, const float* startPos, const float* endPos, dtPolyRef* path, int &pathCount, const int maxPath, std::vector<dtPolyRef> subGraphNodes)
{
    dtAssert(m_nodePool);
	dtAssert(m_openList);

	float cost = 0.0f;
	pathCount = 0;
	
	if (!maxPath)
		return cost;

	m_nodePool->clear();
	m_openList->clear();

	//get start and goal node
	startRef = parentGraph.nodes[startRef].idNode;
	endRef = parentGraph.nodes[endRef].idNode;

	if (startRef == endRef)
	{
		if(numLevel == 0)
			path[0] = startRef + refBase;
		else 
			path[0] = startRef;
		pathCount = 1;
		cost = getCost(&parentGraph.nodes[startRef], startIdPos,endIdPos, startPos, endPos);
		return cost;
	}

	dtNode* startNode = m_nodePool->getNode(startIdPos);
	dtVcopy(startNode->pos, startPos);
	startNode->pidx = 0;
	startNode->cost = 0;
	startNode->total = dtVdist(startPos, endPos) * H_SCALE;
	startNode->id = startIdPos;
	startNode->idPos = startRef;
	startNode->flags = DT_NODE_OPEN;
	m_openList->push(startNode);
	
	dtNode* lastBestNode = startNode;
	float lastBestNodeCost = startNode->total;

	while (!m_openList->empty())
	{
		// Remove node from open list and put it in closed list.
		dtNode* bestNode = m_openList->pop();
		bestNode->flags &= ~DT_NODE_OPEN;
		bestNode->flags |= DT_NODE_CLOSED;
		
		// Reached the goal, stop searching.
		if (bestNode->id == endIdPos)
		{
			lastBestNode = bestNode;
			break;
		}

		Graph::Node* node = &parentGraph.nodes[bestNode->idPos];

		// Get parent poly and tile.
		dtPolyRef parentRef = 0;

		if (bestNode->pidx)
			parentRef = m_nodePool->getNodeAtIdx(bestNode->pidx)->id;

		for(unsigned int i=0; i < node->numEdges; i++)
		{
			dtPolyRef neighbourRef = node->edges[i].idPos;

			if(!subGraphNodes.empty())
			{
				if(std::find(subGraphNodes.begin(), subGraphNodes.end(), neighbourRef)==subGraphNodes.end())
					continue;
			}
		
			if (neighbourRef == parentRef)
				continue;

			dtNode* neighbourNode = m_nodePool->getNode(neighbourRef);

			if(bestNode->id ==neighbourNode->id)
				continue;

			// If the node is visited the first time, calculate node position.
			if (neighbourNode->flags == 0)
			{
				dtVcopy(neighbourNode->pos,  node->edges[i].pos);
			}

			// Calculate cost and heuristic.
			float heuristic = 0.0f;
			//vahid_cost
			// Special case for last node.
			if (neighbourRef == endIdPos)
			{

				const float curCost = getCost(node, bestNode->id, neighbourNode->id, bestNode->pos, neighbourNode->pos);
				Graph::Node* eNode = &parentGraph.nodes[endRef];
				const float endCost = getCost(eNode, neighbourNode->id, endIdPos, neighbourNode->pos, endPos);

				cost = bestNode->cost + curCost + endCost;
				heuristic = 0.0f;
			}
			else
			{
				//const float curCost = dtVdist(bestNode->pos, neighbourNode->pos);

				const float curCost = getCost(node, bestNode->id, neighbourNode->id,bestNode->pos, neighbourNode->pos);

				cost = bestNode->cost + curCost;
				heuristic = dtVdist(neighbourNode->pos, endPos)*H_SCALE;
			}

			const float total = cost + heuristic;
			
			// The node is already in open list and the new result is worse, skip.
			if ((neighbourNode->flags & DT_NODE_OPEN) && total >= neighbourNode->total)
				continue;
			// The node is already visited and process, and the new result is worse, skip.
			if ((neighbourNode->flags & DT_NODE_CLOSED) && total >= neighbourNode->total)
				continue;
			
			// Add or update the node.
			neighbourNode->pidx = m_nodePool->getNodeIdx(bestNode);
			neighbourNode->idPos = node->edges[i].targetNodeId;
			neighbourNode->id = neighbourRef;
			neighbourNode->flags = (neighbourNode->flags & ~DT_NODE_CLOSED);
			neighbourNode->cost = cost;
			neighbourNode->total = total;
			
			if (neighbourNode->flags & DT_NODE_OPEN)
			{
				// Already in open, update node location.
				m_openList->modify(neighbourNode);
			}
			else
			{
				// Put the node in open list.
				neighbourNode->flags |= DT_NODE_OPEN;
				m_openList->push(neighbourNode);
			}
			
			// Update nearest node to target so far.
			if (heuristic < lastBestNodeCost)
			{
				lastBestNodeCost = heuristic;
				lastBestNode = neighbourNode;
			}
		}

	}
	
	// Reverse the path.
	dtNode* prev = 0;
	dtNode* node = lastBestNode;
	cost = lastBestNode->cost;
	do
	{
		dtNode* next = m_nodePool->getNodeAtIdx(node->pidx);
		node->pidx = m_nodePool->getNodeIdx(prev);
		prev = node;
		node = next;
	}
	while (node);
	
	// Store path
	node = prev;
	int n = 0;
	do
	{
		if(numLevel == 0)
			path[n++] = node->idPos + refBase;
		else
			path[n++] = node->idPos;
		node = m_nodePool->getNodeAtIdx(node->pidx);

			//std::ofstream fout("pa.txt", std::ios_base::app | std::ios_base::out);
			//fout<<"\n"<<path[n];

	}
	while (node);
	
	pathCount = n-1;
	
	return cost;
	//vvv
	

}

void Navigation::setGraph()
{
	if( level < numGraphs)
	{
		currentGraph =graphs[level];
	}
	else
	{
		currentGraph =graphs[0];
	}
}


Graph::Node *Navigation::getNode(dtPolyRef ref, int l)
{
	if(l == level)
		return &graphs[l].nodes[ref];

	Graph::Node *node = &graphs[l].nodes[ref];
	
	if(node->edges <= 0)
		return NULL;

	l++;
	return getNode(node->idParent,l);	//OJO NURIA NO HABIA RETURN Y PETABA EN RELEASE CON DEBUGGING INFO
}

void Navigation::checkPartition(int* part, const int numNodes, const int numParts)
{

	int* newPart = (int*) dtAlloc( sizeof(int)*numNodes, DT_ALLOC_TEMP );
	bool* used = (bool*) dtAlloc( sizeof(bool)*numParts, DT_ALLOC_TEMP );
	int nextPart = numParts;

	memset(used, 0, sizeof(bool)*numParts );
	for(int i=0; i < numNodes; i++)
		newPart[i] = -1;

	for(int i=0; i < numNodes; i++)
	{
		if( newPart[i] != -1 )
			continue;

		if( !used[ part[i] ] )
		{
			newPart[i] = part[i];
			used[ part[i] ] = true;			
		}
		else
		{
			newPart[i] = nextPart++;
		}
		explorePartition(i, newPart, part);
	}

	


	memcpy(part, newPart, sizeof(int)*numNodes );
	dtFree(newPart );
	dtFree( used );
}

void Navigation::explorePartition(int idNode, int* newPart, int* part)
{
	Graph::Node *node = &parentGraph.nodes[idNode];
	
	for(unsigned int i=0; i < node->numEdges; i++)
	{
		dtPolyRef neighbourRef = node->edges[i].targetNodeId;

		Graph::Node *neighbour = &parentGraph.nodes[neighbourRef];

		int neighbourID = neighbour->idNode;

		if( neighbourID == -1 )
			continue;

		if( part[idNode] != part[ neighbourID ] )
			continue;

		if( newPart[neighbourID] != -1 )
			continue;

		newPart[neighbourID] = newPart[idNode];
		explorePartition(neighbourID, newPart, part);
	}
}

//=========================================================================

dtStatus Navigation::findPathNav3(rcContext* ctx, dtPolyRef startRef2, dtPolyRef endRef2,const float* startPos2, const float* endPos2, const dtQueryFilter* filter2, dtPolyRef* path2, int &pathCount2, const int maxPath2)
{
	rcAssert(ctx);
	
	Graph::Node *sNode =  getNode(startRef2-refBase,0);
	Graph::Node *eNode =  getNode(endRef2-refBase,0);

	if(sNode == NULL || eNode==NULL)
		return DT_FAILURE;
	 
	std::ofstream fout("v4_path.txt", std::ios_base::app | std::ios_base::out);
		fout<<"\n";
		 
		
	

//float cost = findPathLocal(startRef2, endRef2,   ,   ,startPos2, endPos2, path2, &pathCount2, MAX_POLYstd::vector<dtPolyRef> subGraphNodes);				
 float cost2=m_navQuery->v_findPath(startRef2, endRef2, startPos2, endPos2, filter2, path2, &pathCount2, MAX_POLYS);
//float cost2 = findPath(startRef2, endRef2, startPos2, endPos2,path2, pathCount2, MAX_POLYS);	
 
int cost3=0;
		 for(int i=0;i<pathCount2;i++)
		{
			 
			if(pathCount2>1)
			fout<<path2[i]<<"  ";
		}


		 if(pathCount2>1){
			 cost3=cost2;
		 /*
		 if(pathCount2==1){
			  cost3=0;}
		 else {
			  cost3=cost2;
		 }
		 */


		 fout<<cost3;
		 }


		fout.close();
	
return DT_SUCCESS;
}


//==========================================================
vector<dtPolyRef> split(const string &s, char delim) {
    vector<dtPolyRef> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {

		dtPolyRef d=atoi(item.c_str());
        elems.push_back(d);



    }
    return elems;
}


//============================================
 void Navigation::savepath()
 {  
	// std::ofstream fout("v4.txt", std::ios_base::app | std::ios_base::out);
     string line;
	 if(v3==1){
		
		//ifstream myfile ("v4_path_cityIsland.txt");
    ifstream myfile ("v4_path.txt");
		// ifstream myfile ("v5_path_big.txt");

    if (myfile.is_open())
    {
	//	fout<<"1";
        while ( getline (myfile,line) )
        {
            int idx=line.find(' ');
            string s=line.substr(0,idx);
            int k=atoi(s.c_str());
           dtPolyRef k2=k;
            
            vector<dtPolyRef> xx = split(line ,' ');
            
            std::pair<dtPolyRef,vector<dtPolyRef>>  newNode (k2,xx);
            myMap2.insert (newNode);
            
		
        }
        myfile.close();
    }
	 } 


	 else {
	 
	  if(v3==2){
    ifstream myfile ("v4_path.txt");
		 // ifstream myfile ("v4_path_cityIsland.txt");
		 //  ifstream myfile ("v5_path_big.txt");
    if (myfile.is_open())
    {
		 //fout<<"2";
        while ( getline (myfile,line) )
        {
            int idx=line.find(' ');
            string s=line.substr(0,idx);
            int k=atoi(s.c_str());
           dtPolyRef k2=k;
            
            vector<dtPolyRef> xx = split(line ,' ');
            
            std::pair<dtPolyRef,vector<dtPolyRef>>  newNode (k2,xx);
            myMap2.insert (newNode);
            
		
        }
        myfile.close();
    }
	 } 
	 
	 
	 }
   // else cout<< "Unable to open file";
  
 
    
 
 /*
       string line;
      ifstream myfile ("file4.txt");
      if (myfile.is_open())
      {
        while ( getline (myfile,line) )
        {
       int idx=line.find(' ');
	   int idx2=line.find(".");
        string s=line.substr(0,idx);
		string e=line.substr(idx2-2);
       int k=atoi(s.c_str());
	   double p=atof(e.c_str());
	    vector<int> xx = split(line ,' ');
		
		std::pair< vector<int>, double> p2;
		p2 = std::make_pair(xx,p);
		myMap.insert(std::make_pair<int, std::pair<vector<int>,double>>(k, p2));
		 myfile.close();
      }
    	 
  } // endof if 

*/
	   
 
 } // end of function
    



void Navigation::test_start(Graph::Node *node, dtPolyRef startRef, const float *startPos, int startIdPos)
{
	
//	mutex.lock();

 	int k=0;
 
	//std::ofstream fout("v4.txt", std::ios_base::app | std::ios_base::out);
	auto it2 = myMap2.equal_range(startRef);
    for (auto it = it2.first; it != it2.second; ++it) {
		int m_npolys = 0;	
				int n=0;
					//k=0;	 
			 dtPolyRef m_polys[MAX_POLYS];
		int j = it->second.size();
		 
       for(int i=0;i<j-1;i++){
		   if(it->second[i]>0){
		  m_polys[m_npolys++] =it->second[i];
		//  fout <<it->second[i]<<" ";
		  	   }
		}
	   
	 
	  currentGraph.AddIntraEdge(node->idNode,startIdPos, node->edges[k].idPos,  it->second[j-1], m_polys ,m_npolys);  //n=m_npolys
		 		 k++;
				 
	


	}
	   
	
 // mutex.unlock();

} // end





//============================================================


void Navigation::test_end(Graph::Node *node, dtPolyRef endRef, const float *endPos, int endIdPos)
{
	 // mutex.lock();
 	int k=0;
 

	//int s=node->numEdges;
	 
	//std::ofstream fout("v4.txt", std::ios_base::app | std::ios_base::out);
	auto it2 = myMap2.equal_range(endRef);
    for (auto it = it2.first; it != it2.second; ++it) {
		int m_npolys = 0;	
				int n=0;
				//k=0;		 
			 dtPolyRef m_polys[MAX_POLYS];
		int j = it->second.size();
		 
       for(int i=0;i<j-1;i++){
		   if(it->second[i]>0){
		  m_polys[m_npolys++] =it->second[i];
		//  fout <<it->second[i]<<" ";
		  	   }
		}
	   
 
	   currentGraph.AddIntraEdge(node->idNode,endIdPos, node->edges[k].idPos,  it->second[j-1], m_polys ,m_npolys);  //n=m_npolys
 
	   k++;
	   
	

	} 

	//  mutex.unlock();
}




 
 



