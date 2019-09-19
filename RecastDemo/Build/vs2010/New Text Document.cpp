#define X_MAX 1000
#define X_STEP 20
#define Y_MAX 500
#define Y_STEP 20


struct Node
{
    int id;
	float pos[3];
    int parent_id;
	float perent_pos[3];
 
    float gCost;
    float hCost; 
    float fCost;
};

//============================================================ 

inline bool operator < (const Node& lhs, const Node& rhs)
{//We need to overload "<" to put our struct into a set
    return lhs.fCost < rhs.fCost;
}

//============================================================= 

static bool isDestination(int id, int id_dest) {
        if (id == id_dest) {
            return true;
        }
        return false;
    }
//============================================================= 

static double calculateH(float x, float y, Node dest) {
        double H = (sqrt((x - dest.pos[0])*(x - dest.pos[0])
            + (y - dest.pos[2])*(y - dest.pos[2])));
        return H;
    }
//============================================================= 





 static vector<Node> aStar(Node player, Node dest) {    //Node player, Node dest
        vector<Node> empty;
       // if (isValid(dest.x, dest.y) == false) {
        //    cout << "Destination is an obstacle" << endl;
         //   return empty;
            //Destination is invalid
      //  }
        if (isDestination(player.id, dest.id)) {
           // cout << "You are the destination" << endl;
            return empty;
            //You clicked on yourself
        }
       

	  // bool closedList[(X_MAX / X_STEP)][(Y_MAX / Y_STEP)];
	   
	   bool closedList[(totalNodes)];

	   

        //Initialize whole map
        //Node allMap[50][25];
		
		Node allMap[totalNodes];
 //array<array < Node, totalNodes> > allMap;
        for (int x = 0; x < totalNodes; x++) {
            
                allMap[x].fCost = FLT_MAX;
                allMap[x].gCost = FLT_MAX;
                allMap[x].hCost = FLT_MAX;
                allMap[x].parent_id = -1;
                allMap[x].pos[0] = hexNode[x][2];
                allMap[x].pos[2] = hexNode[x][4];
				allMap[x].id=hexNode[x][0];
                closedList[x] = false;
             
        }

        //Initialize our starting list
        float x = player.pos[0];
        float y = player.pos[2];
		int played_id=player.id;
		allMap[played_id].fCost = 0.0;
        allMap[played_id].gCost = 0.0;
        allMap[played_id].hCost = 0.0;
		allMap[played_id].parent_id = played_id;
       

        vector<Node> openList;  
        openList.emplace_back(allMap[played_id]);
        bool destinationFound = false;


        while (!openList.empty()&&openList.size()<totalNodes) {
            Node node;
            do {
           
                float temp = FLT_MAX;
                vector<Node>::iterator itNode;
                for (vector<Node>::iterator it = openList.begin();
                    it != openList.end(); it = next(it)) {
                    Node n = *it;
                    if (n.fCost < temp) {
                        temp = n.fCost;
                        itNode = it;
                    }
                }
                node = *itNode;
                openList.erase(itNode);
            } while (isValid(node.id) == false);




            x = node.pos[0];
            y = node.[2];
            closedList[node.id] = true;

            //For each neighbour starting from North-West to South-East
           
                    double gNew, hNew, fNew;
											
						vector<int>::iterator i;
						for (i = adj[node.id].begin(); i != adj[node.id].end(); ++i){
							   double gNew, hNew, fNew;
							 int hid = hexNode[*i][0];
							if (isDestination(hid , dest.id))
                        {
                            //Destination found - make path
                            allMap[x + newX][y + newY].parentX = x;
                            allMap[x + newX][y + newY].parentY = y;
                            destinationFound = true;
                            return makePath(allMap, dest);
                        }
						else if (closedList[hid] == false)
                        {
                            gNew = node.gCost + 1.0;
                            hNew = calculateH(hexNode[*i][2], hexNode[*i][4];, dest);
                            fNew = gNew + hNew;
                            // Check if this path is better than the one already present
                            if (allMap[hid].fCost == FLT_MAX ||
                                allMap[hid].fCost > fNew)
                            {
                                // Update the details of this neighbour node
                                allMap[hid].fCost = fNew;
                                allMap[hid].gCost = gNew;
                                allMap[hid].hCost = hNew;
                                //allMap[hid].parentX = x;
                               // allMap[hid].parentY = y;
								allMap[hid].parent = node.id;
                                openList.emplace_back(allMap[hid]);
                            }
                        }
						}
						

    }

 
 }
 
//=========================================================================================================

static vector<Node> makePath(array<array<Node, (Y_MAX / Y_STEP)>, (X_MAX / X_STEP)> map, Node dest) {
        try {
            cout << "Found a path" << endl;
            int x = dest.x;
            int y = dest.y;
            stack<Node> path;
            vector<Node> usablePath;

            while (!(map[x][y].parentX == x && map[x][y].parentY == y)
                && map[x][y].x != -1 && map[x][y].y != -1) 
            {
                path.push(map[x][y]);
                int tempX = map[x][y].parentX;
                int tempY = map[x][y].parentY;
                x = tempX;
                y = tempY;

            }
            path.push(map[x][y]);

            while (!path.empty()) {
                Node top = path.top();
                path.pop();
                usablePath.emplace_back(top);
            }
            return usablePath;
        }
        catch(const exception& e){
            cout << e.what() << endl;
        }
    }




To actually use 


//=======================================================================================================


int startID,endID;


Node player;
            player.pos[0] =hexNode[startID][2];
            player.pos[2] = hexNode[startID][4];
			player.id=hexNode[startID][0];




            
Node destination;
            destination.pos[0] = hexNode[endID][2];
            destination.pos[2] = hexNode[endID][4];

            aStar(player, destination);  
               

          

