/*
*  Matthew Buchanan
*  Project 5
*  CS-340
*/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cassert>
#include <sstream>
#include <vector>
using namespace std;

/* Edge Class */
class Edge
{
public:
	int v;
	int weight;

	Edge()
	{
	};

	~Edge()
	{
	};
};

//* Vertex Class */
class Vertex
{
public:
	int id;
	int color; // 0 = white, 1 = gray, 2 = black
	int d;
	int f;
	int parent;
	vector<Edge> adjacencies;
	int heapIndex;
	int spe;

	Vertex()
	{
	};

	~Vertex()
	{
	};
};

/* Graph class */
class Graph
{
public:
	vector<Vertex> vertices;
	vector<Vertex> topSort;// holds results of topological sort
	vector<Vertex> dijkstraResult;
	Vertex * pQueue;// priority queue
	int pQueueSize;
	int time;
	bool isDAG; // flag variable to mark if graph has a cycle
	int maxWeight = 9999;// set infinity value for edge weights here
	int e, v;

	Graph()
	{
		e = v = 0;
	};

	~Graph()
	{
	};

	/* Topological sort function */
	void topologicalSort()
	{
		for (int i = 0; i < vertices.size(); i++)// set all vertices to white
			vertices[i].color = 0;
		isDAG = 1;
		time = 0;
		for (int i = 0; i < vertices.size(); i++)
		{
			if (!(vertices[i].color))
				topVisit(vertices[i]);
		}
	}

	/* Topological sort helper function */
	void topVisit(Vertex &u)//note all back edges will be stored rather than returning false once one is detected
	{
		time++;
		u.d = time;
		u.color = 1;
		for (int i = 0; i < u.adjacencies.size(); i++)//for each adjacency of u
		{
			if (vertices[u.adjacencies[i].v - 1].color == 1)// if adjacency is gray, back edge detected 
				isDAG = false;// set flag for not a DAG
			else if (vertices[u.adjacencies[i].v - 1].color == 0)// // if adajency of u is white recurse on u's adjacent vertex
			{
				//vertices[u.adjacencies[i].v - 1].parent = u.id;
				topVisit(vertices[u.adjacencies[i].v - 1]);
			}
		}
		u.color = 2;// color u black
		time++;
		u.f = time;// finalize timestamp
		topSort.push_back(u);
	}

	/* Priority queue implementation */
	void minHeap()//construct heap by repeated insertion for use as priority queue
	{
		pQueue = new Vertex[vertices.size() + 1];
		pQueueSize = 0;
	}

	/* Heapify for the min heap priority queue */
	void minHeapify(Vertex * a, int i)
	{
		int smallest;
		int left = 2 * i;
		int right = left + 1;
		if (left <= pQueueSize && a[left].spe < a[i].spe)
			smallest = left;
		else
			smallest = i;
		if (right <= pQueueSize && a[right].spe < a[smallest].spe)
			smallest = right;
		if (smallest != i)
		{
			Vertex tmp = a[i];
			a[i] = a[smallest];
			vertices[a[i].id - 1].heapIndex = i;
			a[smallest] = tmp;
			vertices[a[smallest].id - 1].heapIndex = smallest;
			minHeapify(a, smallest);
		}
	}

	/* Pop from min priority queue */
	Vertex extractMin()
	{
		if (pQueueSize > 0)
		{
			Vertex min = pQueue[1];
			pQueue[1] = pQueue[pQueueSize];
			pQueueSize--;
			minHeapify(pQueue, 1);
			vertices[min.id - 1].heapIndex = -1;
			return min;
		}
	}

	/* Push to priority queue */
	void minHeapInsert(Vertex v)
	{
		pQueueSize++;
		pQueue[pQueueSize] = v;
		vertices[pQueue[pQueueSize].id - 1].heapIndex = pQueueSize;//update pointer
		int index = pQueueSize;
		while (vertices[v.id - 1].heapIndex != 1 && v.spe < pQueue[index / 2].spe)
		{
			Vertex tmp = pQueue[index / 2];
			pQueue[index / 2] = v;
			vertices[pQueue[index / 2].id - 1].heapIndex = index / 2;
			pQueue[index] = tmp;
			vertices[pQueue[index].id - 1].heapIndex = index;
			index = index / 2;
		}
	}

	/* Decrease key and bubble up in min heap */
	bool decreaseKey(int index, int key)
	{
		if (key > pQueue[index].spe)
		{
			return false;
		}
		pQueue[index].spe = key;
		while (index > 1 && pQueue[index].spe < pQueue[index / 2].spe)
		{
			Vertex v = pQueue[index];
			pQueue[index] = pQueue[index / 2];
			vertices[pQueue[index].id - 1].heapIndex = index;
			vertices[v.id - 1].heapIndex = index / 2;
			pQueue[index / 2] = v;
			index = index / 2;
		}
		return true;
	}

	void dijkstra(int source)
	{
		minHeap();
		for (int i = 0; i < vertices.size(); i++)
			minHeapInsert(vertices[i]);
		initalizeSingleSourceD(pQueue[vertices[source - 1].heapIndex]);
		while (pQueueSize > 0)
		{
			Vertex u = extractMin();
			dijkstraResult.push_back(u);
			for (int j = 0; j < u.adjacencies.size(); j++)
			{
				if (vertices[u.adjacencies[j].v - 1].heapIndex != -1)//check if adjacent vertex is in pQueue
					relaxD(u, pQueue[vertices[u.adjacencies[j].v - 1].heapIndex], u.adjacencies[j].weight);
			}
		}
		for (int i = 0; i < vertices.size(); i++)//assign results back to vertices
		{
			vertices[dijkstraResult[i].id - 1] = dijkstraResult[i];
		}
	}

	void initalizeSingleSourceD(Vertex &s)
	{
		for (int i = 1; i < vertices.size() + 1; i++)
		{
			pQueue[i].spe = maxWeight;
			pQueue[i].parent = -1;
		}
		decreaseKey(vertices[s.id - 1].heapIndex, 0);
	}

	void relaxD(Vertex &u, Vertex &v, int weight)
	{
		if (v.spe > u.spe + weight)
		{
			v.spe = u.spe + weight;
			v.parent = u.id;
			decreaseKey(vertices[v.id - 1].heapIndex, v.spe);
		}
	}

	bool bellmanFord(int source)
	{
		initalizeSingleSource(vertices[source - 1]);
		for (int i = 0; i < vertices.size() - 1; i++)//relax all edges V - 1 times
		{
			for (int j = 0; j < vertices.size(); j++)
			{
				for (int k = 0; k < vertices[j].adjacencies.size(); k++)
				{
					relax(vertices[j], vertices[vertices[j].adjacencies[k].v - 1], vertices[j].adjacencies[k].weight);
				}
			}
		}
		for (int i = 0; i < vertices.size(); i++)//for each edge E of V
		{
			for (int j = 0; j < vertices[i].adjacencies.size(); j++)
			{
				if (vertices[vertices[i].adjacencies[j].v - 1].spe > vertices[i].spe + vertices[i].adjacencies[j].weight)
					return false;
			}
		}
		return true;
	}

	void initalizeSingleSource(Vertex &s)
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].spe = maxWeight;
			vertices[i].parent = -1;
		}
		s.spe = 0;
	}

	void relax(Vertex &u, Vertex &v, int weight)
	{
		if (u.spe != 9999)//if u.spe is not infinity
		{
			if (v.spe > u.spe + weight)
			{
				v.spe = u.spe + weight;
				v.parent = u.id;
			}
		}
	}

	void dagShortestPath(int source)
	{
		initalizeSingleSource(vertices[source - 1]);
		for (int i = topSort.size() - 1; i >= 0; i--)
		{
			for (int j = 0; j < topSort[i].adjacencies.size(); j++)
				relax(vertices[topSort[i].id - 1], vertices[topSort[i].adjacencies[j].v - 1], topSort[i].adjacencies[j].weight);
		}
	}
};

/* Function prototypes */
void readFile(string f, Graph * g, bool &n);
void displayGraph(Graph g);
void getSource(int &s);
void getDestination(int &d);


/**************** Begin main ****************/
int main()
{
	bool neg = false;
	int source, destination;

	Graph * newGraph = new Graph;
	readFile("graphin-c2.txt", newGraph, neg);
	displayGraph(*newGraph);
	newGraph->topologicalSort();
	getSource(source);
	if (newGraph->isDAG)
	{
		cout << "Graph is a DAG, using DAG shortest paths algorithm." << endl;
		newGraph->dagShortestPath(source);
	}
	else if (!neg)
	{
		cout << "Graph is not a DAG and has no negative edge weights, using Dijkstra's shortest paths algorithm." << endl;
		newGraph->dijkstra(source);
	}
	else
	{
		cout << "Graph is not a DAG and has negative edge weights, using Bellman-Ford's shortest paths algorithm." << endl;
		bool negCycle = !newGraph->bellmanFord(source);
		if (negCycle)
		{
			cout << "Negative weight cycle detected, exiting program" << endl;
			system("PAUSE");
			return 0;
		}
	}
	getDestination(destination);
	while (destination != -1)
	{
		Vertex temp = newGraph->vertices[destination - 1];
		if (temp.id != source && temp.parent == -1)
			cout << "No path from source to destination." << endl;
		else
		{
			cout << "Total path weight is: " << temp.spe << endl << "Nodes in path:" << endl;
			cout << temp.id << endl;
			while (temp.parent != -1)
			{
				temp = newGraph->vertices[temp.parent - 1];
				cout << temp.id << endl;
			}
			cout << endl;
		}
		getDestination(destination);
	}

}
/***************** End Main ******************/

void getSource(int &s)
{
	cout << "Please select a source vertex by I.D" << endl;
	cin >> s;
	cout << endl;
}

void getDestination(int &d)
{
	cout << "Please select a destination vertix by I.D, or enter -1 to quit" << endl;
	cin >> d;
	cout << endl;
}

/* Function to read an input .txt file and build the graph */
void readFile(string f, Graph * g, bool &n)
{
	ifstream in;
	in.open(f);
	assert(in.is_open());
	int i = 0;
	int adjacency;
	string line;
	char temp2;
	while (!in.eof())
	{
		g->v++;
		Vertex newVertex;
		g->vertices.push_back(newVertex);
		in >> g->vertices[i].id >> temp2;//assign node id and discard colon into temp2
		getline(in, line);
		istringstream is(line);
		while (is >> adjacency)//use string stream to parse ints from line of text
		{
			g->e++;
			Edge e;
			e.v = adjacency;
			is >> e.weight;
			if (e.weight < 0)//detect negative edge weights and set flag variable
				n = true;
			g->vertices[i].adjacencies.push_back(e);
		}
		line = "";
		i++;
		is.clear();
	}
	in.close();
	in.clear();
}

/* Function to display the graph in same format as input file */
void displayGraph(Graph g)
{
	cout << "Input graph:" << endl;
	for (int i = 0; i < g.vertices.size(); i++)
	{
		cout << g.vertices[i].id << ": ";
		for (int j = 0; j < g.vertices[i].adjacencies.size(); j++)
		{
			cout << g.vertices[i].adjacencies[j].v << " " << g.vertices[i].adjacencies[j].weight << " ";
		}
		cout << endl;
	}
	cout << endl;
}

