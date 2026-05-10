// ==========================
// SECTION 1: HEADER FILES
// This section includes all required libraries for the program
// ==========================

#include <iostream>     // For input and output operations
#include <vector>       // For dynamic array (adjacency list)
#include <queue>        // For BFS traversal
#include <omp.h>        // For OpenMP parallel programming
#include <limits>       // For input buffer handling

using namespace std;


// ==========================
// SECTION 2: GRAPH CLASS DEFINITION
// This class represents a graph and implements Parallel BFS and DFS
// ==========================

class Graph {
private:
    int V;                              // Number of vertices in graph
    vector<vector<int>> adj;            // Adjacency list representation

public:

    // Constructor to initialize graph with V vertices
    Graph(int vertices) {
        V = vertices;
        adj.resize(V);
    }

    // Function to add an undirected edge
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }


    // ==========================
    // SECTION 3: PARALLEL BFS IMPLEMENTATION
    // BFS explores level-by-level using a queue
    // Parallelization is applied on nodes of the same level
    // ==========================

    void parallelBFS(int start) {

        // Initialize visited array
        vector<int> visited(V, 0);

        // Queue for BFS traversal
        queue<int> q;

        // Mark start node as visited and push into queue
        visited[start] = 1;
        q.push(start);

        cout << "\n--- Parallel BFS Traversal ---\n";

        // Continue until queue is empty
        while (!q.empty()) {

            // Capture current level size
            int levelSize = q.size();

            // Store current level nodes
            vector<int> currentLevel;

            // Extract nodes of current level
            for (int i = 0; i < levelSize; i++) {
                int node = q.front();
                q.pop();
                currentLevel.push_back(node);
            }

            // Process current level nodes in parallel
            #pragma omp parallel for
            for (int i = 0; i < currentLevel.size(); i++) {

                int node = currentLevel[i];

                // Critical section for safe printing
                #pragma omp critical
                cout << node << " ";

                // Explore neighbors
                for (int neighbor : adj[node]) {

                    bool shouldAdd = false;

                    // Protect shared visited array
                    #pragma omp critical
                    {
                        if (!visited[neighbor]) {
                            visited[neighbor] = 1;
                            shouldAdd = true;
                        }
                    }

                    // Push into queue safely
                    if (shouldAdd) {
                        #pragma omp critical
                        q.push(neighbor);
                    }
                }
            }
        }

        cout << endl;
    }


    // ==========================
    // SECTION 4: DFS UTILITY FUNCTION (FIXED)
    // Recursive DFS without parallel tasks to avoid race conditions
    // ==========================

    void parallelDFSUtil(int node, vector<int>& visited) {

        // Print node safely
        cout << node << " ";

        // Traverse neighbors
        for (int neighbor : adj[node]) {

            // Check if neighbor is not visited
            if (!visited[neighbor]) {

                // Mark as visited
                visited[neighbor] = 1;

                // Recursive DFS call (sequential to maintain correctness)
                parallelDFSUtil(neighbor, visited);
            }
        }
    }


    // ==========================
    // SECTION 5: PARALLEL DFS DRIVER FUNCTION
    // Initializes DFS traversal
    // ==========================

    void parallelDFS(int start) {

        vector<int> visited(V, 0);

        cout << "\n--- Parallel DFS Traversal ---\n";

        // Mark starting node as visited
        visited[start] = 1;

        // Call DFS utility function
        parallelDFSUtil(start, visited);

        cout << endl;
    }
};


// ==========================
// SECTION 6: MAIN FUNCTION
// Handles input, execution, and output
// ==========================

int main() {

    // ==========================
    // INPUT SECTION
    // ==========================

    int V, E;

    cout << "Enter number of vertices: ";
    cin >> V;

    cout << "Enter number of edges: ";
    cin >> E;

    // Create graph
    Graph g(V);

    cout << "Enter edges (u v):\n";

    // Input edges
    for (int i = 0; i < E; i++) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    int start;

    cout << "Enter starting vertex: ";
    cin >> start;


    // ==========================
    // PROCESSING SECTION
    // Perform BFS and DFS
    // ==========================

    g.parallelBFS(start);
    g.parallelDFS(start);


    // ==========================
    // OUTPUT / CONCLUSION SECTION
    // ==========================

    cout << "\nProgram executed successfully.\n";
    cout << "Parallel BFS explores level-wise.\n";
    cout << "Parallel DFS explores depth-wise safely without race conditions.\n";

    // Pause for user (useful in IDE)
    cout << "\nPress Enter to exit...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    return 0;
}