#include <iostream>
#include <vector>
#include <sstream>
#include <map>

const int INF = 1e9;

class NetworkTopology {
private:
    int count = 0;
    map<pair<int, int>, int> edges;
    int adjacencyMatrix[MAX_N][MAX_N] = {-1};
public:
    NetworkTopology() {
        // init topologies
        for (auto &row: adjacencyMatrix) {
            for (int &j: row) {
                j = -1;
            }
        }
    }

    void addEdge(int source, int destination, int cost) {
        if (source == destination) {
            throw invalid_argument("source and destination cannot be the same");
        }
        if (adjacencyMatrix[source][destination] != -1 || adjacencyMatrix[destination][source] != -1) {
            throw invalid_argument("topology already set");
        }
        adjacencyMatrix[source][destination] = adjacencyMatrix[destination][source] = cost;
        adjacencyMatrix[source][source] = adjacencyMatrix[destination][destination] = 0;
        edges.insert(make_pair(make_pair(source, destination), cost));
        edges.insert(make_pair(make_pair(destination, source), cost));
        count = max(count, max(source, destination));
    }

    void removeEdge(int source, int dest) {
        if (adjacencyMatrix[source][dest] == -1) {
            cout << "No such edge exists" << endl;
            return;
        }
        adjacencyMatrix[source][dest] = adjacencyMatrix[dest][source] = -1;
        edges.erase(make_pair(source, dest));
        edges.erase(make_pair(dest, source));
    }

    void modifyEdge(int source, int dest, int cost) {
        count = max(count, max(source, dest));
        if (adjacencyMatrix[source][dest] == -1) {
            adjacencyMatrix[source][dest] = adjacencyMatrix[dest][source] = cost;
            adjacencyMatrix[source][source] = adjacencyMatrix[dest][dest] = 0;
            edges.insert(make_pair(make_pair(source, dest), cost));
            edges.insert(make_pair(make_pair(dest, source), cost));
        } else {
            adjacencyMatrix[source][dest] = adjacencyMatrix[dest][source] = cost;
            edges[make_pair(source, dest)] = cost;
            edges[make_pair(dest, source)] = cost;
        }
    }

    void printAdjacencyMatrix() {
        stringstream ss;
        ss << "u|v\t|\t";
        for (int i = 0; i < count; i++) {
            ss << i + 1 << "\t";
        }
        ss << endl;
        for (int i = 0; i < 3 * count + 1; i++) {
            ss << "-";
        }
        ss << endl;
        for (int i = 0; i < count; i++) {
            ss << i + 1 << "\t|\t";
            for (int j = 0; j < count; j++) {
                ss << adjacencyMatrix[i + 1][j + 1] << "\t";
            }
            ss << endl;
        }
        cout << ss.str();
    }

    int getCount() {
        return count;
    }

    void dijkstra(int src_node) {
        if (src_node < 1 || src_node > count) {
            throw invalid_argument("invalid source node");
        }
        int distances[count + 1];
        bool test[count + 1];
        int parent[count + 1];
        for (int i = 1; i < count + 1; i++) {
            distances[i] = INF;
            parent[i] = -1;
            test[i] = false;
        }
        string delim;
        distances[src_node] = 0;
        int iterate = 1;
        for (int i = 1; i < count + 1; i++) {
            if (i == src_node)
                continue;
            int m = min_distance(distances, test);
            test[m] = true;
            for (int j = 1; j < count + 1; ++j) {
                if (!test[j] && adjacencyMatrix[m][j] && adjacencyMatrix[m][j] != -1 && distances[m] != INF &&
                    distances[m] + adjacencyMatrix[m][j] < distances[j]) {
                    distances[j] = distances[m] + adjacencyMatrix[m][j];
                    parent[j] = m;
                }
            }
            stringstream ss;
            ss << delim;
            ss << "Iter " << iterate << ": " << endl;
            ss << "Dest\t|\t";
            for (int j = 1; j < count + 1; j++) {
                ss << j << "\t|\t";
            }
            ss << endl;
            ss << "Cost\t|\t";
            for (int j = 1; j < count + 1; j++) {
                if (distances[j] == INF) {
                    ss << "-1";
                } else {
                    ss << distances[j];
                }
                ss << "\t|\t";
            }
            ss << endl;
            cout << ss.str();
            delim = "-------------------------------------------------\n";
            iterate++;
        }
        stringstream ss;
        ss << delim << "Path\t\t\t|\tMin-Cost\t|\tShortest Path\n";
        ss << "-------------------------------------------------\n";
        for (int i = 1; i < count + 1; i++) {
            if (i != src_node) {
                ss << src_node << "  ->  " << i << "\t\t|\t\t" << distances[i] << "\t\t|\t";
                print_parents(parent, i, ss);
                ss << i << endl;
            }
        }
        cout << ss.str();
    }

    void print_parents(const int *parent, int j, stringstream &ss) {
        if (parent[j] != -1) {
            print_parents(parent, parent[j], ss);
            ss << parent[j] << " -> ";
        }
    }

    int min_distance(const int distances[], const bool test[]) {
        int minimum = INF;
        int min_index;
        for (int i = 1; i < count + 1; ++i) {
            if (!test[i] && distances[i] <= minimum) {
                minimum = distances[i];
                min_index = i;
            }
        }
        return min_index;
    }

    void bellman_ford(int src_node) {
        if (src_node < 1 || src_node > count) {
            throw invalid_argument("invalid source node");
        }
        int distances[count + 1];
        int parent[count + 1];
        for (int i = 0; i < count + 1; i++) {
            distances[i] = INF;
            parent[i] = -1;
        }
        distances[src_node] = 0;
        for (int i = 0; i < count - 1; i++) {
            for (const auto &item: edges) {
                int src = item.first.first;
                int dest = item.first.second;
                int cost = item.second;
                if (distances[src] != INF && distances[src] + cost < distances[dest]) {
                    distances[dest] = distances[src] + cost;
                    parent[dest] = src;
                }
            }
        }
        stringstream ss;
        ss << "Dest\t|\tNext Hop\t|\tDist\t|\tShortest Path\n";
        ss << "-----------------------------------------------------------------------\n";
        for (int i = 1; i < count + 1; i++) {
            auto next_hop = (parent[i] == -1) ? src_node : parent[i];
            ss << i << "\t\t|\t" << next_hop << "\t\t\t|\t" << distances[i] << "\t\t|\t" << "[";
            print_parents(parent, i, ss);
            ss << i << "]\n";
        }
        cout << ss.str();
    }

};

int main() {
    return 0;
}
