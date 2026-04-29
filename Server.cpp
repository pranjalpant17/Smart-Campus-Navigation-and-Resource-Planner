#include "models.h"
#include "graph.h"
#include "campus_data.h"
#include "bfs.h"
#include "dfs.h"
#include "dijkstra.h"
#include "allocator.h"

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <functional>
#include <cstring>

// ── Platform socket includes ──────────────────────────────────
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>

  typedef int socklen_t;
  #define CLOSE_SOCKET(s) closesocket(s)
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <unistd.h>
  #define CLOSE_SOCKET(s) close(s)
  typedef int SOCKET;
  #define INVALID_SOCKET -1
#endif

using namespace std;

// ── Global state ──────────────────────────────────────────────
CampusGraph* G_ptr = nullptr;
ResourceAllocator* RA_ptr = nullptr;

// ════════════════════════════════════════════════════════════
//  JSON HELPERS
// ════════════════════════════════════════════════════════════

string jsonStr(const string& s) {
    // Escape quotes inside the string
    string out = "\"";
    for(char c : s) {
        if(c == '"') out += "\\\"";
        else         out += c;
    }
    return out + "\"";
}

string jsonArray(const vector<string>& v) {
    string out = "[";
    for(int i = 0; i < (int)v.size(); i++) {
        out += jsonStr(v[i]);
        if(i + 1 < (int)v.size()) out += ",";
    }
    return out + "]";
}

// ════════════════════════════════════════════════════════════
//  URL PARSING
// ════════════════════════════════════════════════════════════

// Parse URL like "/bfs?src=0&dst=10"
// Returns: path="/bfs", params={"src":"0","dst":"10"}
struct ParsedURL {
    string path;
    map<string,string> params;
};

ParsedURL parseURL(const string& url) {
    ParsedURL result;
    size_t q = url.find('?');
    result.path = (q == string::npos) ? url : url.substr(0, q);

    if(q != string::npos) {
        string query = url.substr(q + 1);
        stringstream ss(query);
        string token;
        while(getline(ss, token, '&')) {
            size_t eq = token.find('=');
            if(eq != string::npos)
                result.params[token.substr(0, eq)] = token.substr(eq + 1);
        }
    }
    return result;
}

int getParam(const map<string,string>& p, const string& key, int def = 0) {
    auto it = p.find(key);
    return (it != p.end()) ? stoi(it->second) : def;
}

string getParamStr(const map<string,string>& p, const string& key, const string& def = "") {
    auto it = p.find(key);
    return (it != p.end()) ? it->second : def;
}

// ════════════════════════════════════════════════════════════
//  ROUTE HANDLERS — each returns a JSON string
// ════════════════════════════════════════════════════════════

// GET /buildings → list every building as JSON array
string handleBuildings() {
    string out = "[";
    bool first = true;
    for (auto &pair : G_ptr->buildings) {
    int id = pair.first;
    auto &b = pair.second;

    if (!first) out += ",";
    first = false;

    out += "{\"id\":" + to_string(id)
         + ",\"name\":" + jsonStr(b.name) + "}";
}
    return out + "]";
}

// GET /graph → adjacency list as JSON
string handleGraph() {
    string out = "[";
    bool first = true;
    for(int i = 0; i < G_ptr->V; i++) {
        if(!G_ptr->buildings.count(i)) continue;
        if(!first) out += ",";
        first = false;
        out += "{\"id\":" + to_string(i)
             + ",\"name\":" + jsonStr(G_ptr->buildingName(i))
             + ",\"neighbours\":[";
        bool nf = true;
        for(auto& e : G_ptr->getNeighbours(i)) {
            if(!nf) out += ",";
            nf = false;
            out += "{\"to\":" + to_string(e.to)
                 + ",\"name\":" + jsonStr(G_ptr->buildingName(e.to))
                 + ",\"weight\":" + to_string(e.weight)
                 + ",\"accessible\":" + (e.isAccessible?"true":"false") + "}";
        }
        out += "]}";
    }
    return out + "]";
}

// GET /bfs?src=0&dst=10
string handleBFS(const map<string,string>& params) {
    int src = getParam(params, "src", 0);
    int dst = getParam(params, "dst", 10);

    // Capture cout output by redirecting
    // (we run BFS directly and build JSON ourselves)
    vector<bool> visited(G_ptr->V, false);
    vector<int>  parent(G_ptr->V, -1);
    queue<int>   q;
    vector<string> visitOrder;

    visited[src] = true;
    q.push(src);

    while(!q.empty()) {
        int u = q.front(); q.pop();
        visitOrder.push_back(G_ptr->buildingName(u));
        if(u == dst) break;
        for(auto& e : G_ptr->getNeighbours(u)) {
            if(!visited[e.to]) {
                visited[e.to] = true;
                parent[e.to]  = u;
                q.push(e.to);
            }
        }
    }

    // Reconstruct path
    vector<string> path;
    for(int cur = dst; cur != -1; cur = parent[cur])
        path.push_back(G_ptr->buildingName(cur));
    reverse(path.begin(), path.end());

    bool found = (!path.empty() && path[0] == G_ptr->buildingName(src));

    string out = "{";
    out += "\"found\":" + string(found ? "true" : "false");
    out += ",\"src\":" + jsonStr(G_ptr->buildingName(src));
    out += ",\"dst\":" + jsonStr(G_ptr->buildingName(dst));
    out += ",\"visitOrder\":" + jsonArray(visitOrder);
    out += ",\"path\":" + (found ? jsonArray(path) : "[]");
    out += ",\"hops\":" + to_string(found ? (int)path.size()-1 : -1);
    out += "}";
    return out;
}

// GET /dfs?src=0&dst=10
string handleDFS(const map<string,string>& params) {
    int src = getParam(params, "src", 0);
    int dst = getParam(params, "dst", 10);

    const size_t MAX_PATHS = 20;
    vector<vector<int>> allPaths;
    vector<bool> visited(G_ptr->V, false);
    vector<int>  cur;

    function<void(int)> dfs = [&](int node) {
        if(allPaths.size() >= MAX_PATHS) return;
        visited[node] = true;
        cur.push_back(node);
        if(node == dst) {
            allPaths.push_back(cur);
        } else {
            for(auto& e : G_ptr->getNeighbours(node))
                if(!visited[e.to]) dfs(e.to);
        }
        cur.pop_back();
        visited[node] = false;
    };
    dfs(src);

    string out = "{\"src\":" + jsonStr(G_ptr->buildingName(src));
    out += ",\"dst\":" + jsonStr(G_ptr->buildingName(dst));
    out += ",\"count\":" + to_string(allPaths.size());
    out += ",\"paths\":[";
    for(int i = 0; i < (int)allPaths.size(); i++) {
        if(i) out += ",";
        vector<string> names;
        for(int id : allPaths[i]) names.push_back(G_ptr->buildingName(id));
        out += "{\"hops\":" + to_string((int)allPaths[i].size()-1)
             + ",\"path\":" + jsonArray(names) + "}";
    }
    out += "]}";
    return out;
}

// GET /dijkstra?src=0&dst=20
string handleDijkstra(const map<string,string>& params) {
    int src = getParam(params, "src", 0);
    int dst = getParam(params, "dst", 20);

    vector<int> dist(G_ptr->V, INT_MAX);
    vector<int> parent(G_ptr->V, -1);
    using pii = pair<int,int>;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    dist[src] = 0;
    pq.push({0, src});

    while(!pq.empty()) {
        auto top = pq.top();
        pq.pop();

        int d = top.first;
        int u = top.second;
        if(d > dist[u]) continue;
        if(u == dst) break;
        for(auto& e : G_ptr->getNeighbours(u)) {
            int nd = dist[u] + e.weight;
            if(nd < dist[e.to]) {
                dist[e.to] = nd;
                parent[e.to] = u;
                pq.push({nd, e.to});
            }
        }
    }

    // Reconstruct path
    vector<string> path;
    vector<int>    weights;
    vector<int>    rawPath;
    for(int c = dst; c != -1; c = parent[c]) rawPath.push_back(c);
    reverse(rawPath.begin(), rawPath.end());
    bool found = (!rawPath.empty() && rawPath[0] == src);
    for(int id : rawPath) path.push_back(G_ptr->buildingName(id));
    for(int i = 0; i+1 < (int)rawPath.size(); i++)
        weights.push_back(G_ptr->edgeWeight(rawPath[i], rawPath[i+1]));

    // All distances
    string allDist = "[";
    vector<pair<int,int>> rows;
    for(int i = 0; i < G_ptr->V; i++)
        if(G_ptr->buildings.count(i) && dist[i] != INT_MAX)
            rows.push_back({dist[i], i});
    sort(rows.begin(), rows.end());
    for(int i = 0; i < (int)rows.size(); i++) {
        if(i) allDist += ",";
        allDist += "{\"id\":" + to_string(rows[i].second)
                 + ",\"name\":" + jsonStr(G_ptr->buildingName(rows[i].second))
                 + ",\"dist\":" + to_string(rows[i].first) + "}";
    }
    allDist += "]";

    // Edge weights array
    string wArr = "[";
    for(int i = 0; i < (int)weights.size(); i++) {
        if(i) wArr += ",";
        wArr += to_string(weights[i]);
    }
    wArr += "]";

    string out = "{\"found\":" + string(found?"true":"false");
    out += ",\"src\":" + jsonStr(G_ptr->buildingName(src));
    out += ",\"dst\":" + jsonStr(G_ptr->buildingName(dst));
    out += ",\"path\":" + (found ? jsonArray(path) : "[]");
    out += ",\"weights\":" + wArr;
    out += ",\"totalDist\":" + to_string(found ? dist[dst] : -1);
    out += ",\"allDist\":" + allDist;
    out += "}";
    return out;
}

// GET /resources?id=4
string handleResources(const map<string,string>& params) {
    int id = getParam(params, "id", 4);
    if(!G_ptr->buildings.count(id))
        return "{\"error\":\"Building not found\"}";

    auto& b = G_ptr->buildings[id];
    string out = "{\"building\":" + jsonStr(b.name)
               + ",\"id\":" + to_string(id)
               + ",\"resources\":[";
    bool first = true;
    for(auto& r : b.resources) {
        if(!first) out += ",";
        first = false;
        out += "{\"name\":" + jsonStr(r->name)
             + ",\"type\":" + jsonStr(r->getType())
             + ",\"capacity\":" + to_string(r->capacity)
             + ",\"occupied\":" + (r->isOccupied?"true":"false") + "}";
    }
    return out + "]}";
}

// GET /allocate?id=4&cap=50
string handleAllocate(const map<string,string>& params) {
    int id  = getParam(params, "id",  4);
    int cap = getParam(params, "cap", 50);
    if(!G_ptr->buildings.count(id))
        return "{\"success\":false,\"msg\":\"Building not found\"}";

    auto& b = G_ptr->buildings[id];
    for(auto& r : b.resources) {
        if(r->getType() != "Lab" && !r->isOccupied && r->capacity >= cap) {
            r->isOccupied = true;
            RA_ptr->saveToFile();
            return "{\"success\":true,\"room\":" + jsonStr(r->name)
                 + ",\"building\":" + jsonStr(b.name)
                 + ",\"capacity\":" + to_string(r->capacity)
                 + ",\"type\":" + jsonStr(r->getType()) + "}";
        }
    }
    return "{\"success\":false,\"msg\":\"No suitable room found\"}";
}

// GET /deallocate?id=4&room=CR1
string handleDeallocate(const map<string,string>& params) {
    int id      = getParam(params, "id", 4);
    string room = getParamStr(params, "room");
    if(!G_ptr->buildings.count(id))
        return "{\"success\":false,\"msg\":\"Building not found\"}";

    auto& b = G_ptr->buildings[id];
    for(auto& r : b.resources) {
        if(r->name == room) {
            if(!r->isOccupied)
                return "{\"success\":false,\"msg\":\"Already free\"}";
            r->isOccupied = false;
            RA_ptr->saveToFile();
            return "{\"success\":true,\"room\":" + jsonStr(room)
                 + ",\"building\":" + jsonStr(b.name) + "}";
        }
    }
    return "{\"success\":false,\"msg\":\"Room not found\"}";
}

// GET /allocate_lab?id=4&cap=50
string handleAllocateLab(const map<string,string>& params) {
    int id  = getParam(params, "id",  4);
    int cap = getParam(params, "cap", 50);
    if(!G_ptr->buildings.count(id))
        return "{\"success\":false,\"msg\":\"Building not found\"}";

    auto& b = G_ptr->buildings[id];
    for(auto& r : b.resources) {
        if(r->getType() == "Lab" && !r->isOccupied && r->capacity >= cap) {
            r->isOccupied = true;
            RA_ptr->saveToFile();
            return "{\"success\":true,\"room\":" + jsonStr(r->name)
                 + ",\"building\":" + jsonStr(b.name)
                 + ",\"capacity\":" + to_string(r->capacity)
                 + ",\"type\":" + jsonStr(r->getType()) + "}";
        }
    }
    return "{\"success\":false,\"msg\":\"No suitable lab found in this building\"}";
}

// GET /nearest?src=0&cap=50
string handleNearest(const map<string,string>& params, bool labOnly = false) {
    int src = getParam(params, "src", 0);
    int cap = getParam(params, "cap", 50);

    vector<int> dist(G_ptr->V, INT_MAX);
    using pii = pair<int,int>;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    dist[src] = 0;
    pq.push({0, src});

    while(!pq.empty()) {
        auto top = pq.top();
        pq.pop();

        int d = top.first;
        int u = top.second;
        if(d > dist[u]) continue;
        if(G_ptr->buildings.count(u)) {
            auto& b = G_ptr->buildings[u];
            for(auto& r : b.resources) {
                bool typeOk = !labOnly || (r->getType() == "Lab");
                if(typeOk && !r->isOccupied && r->capacity >= cap) {
                    r->isOccupied = true;
                    RA_ptr->saveToFile();
                    return "{\"success\":true"
                           ",\"room\":" + jsonStr(r->name) +
                           ",\"building\":" + jsonStr(b.name) +
                           ",\"buildingId\":" + to_string(u) +
                           ",\"distance\":" + to_string(d) +
                           ",\"capacity\":" + to_string(r->capacity) +
                           ",\"type\":" + jsonStr(r->getType()) + "}";
                }
            }
        }
        for(auto& e : G_ptr->getNeighbours(u)) {
            int nd = d + e.weight;
            if(nd < dist[e.to]) { dist[e.to] = nd; pq.push({nd, e.to}); }
        }
    }
    return "{\"success\":false,\"msg\":\"No resource found on campus\"}";
}

// ════════════════════════════════════════════════════════════
//  HTTP SERVER  (minimal, single-threaded, for localhost)
// ════════════════════════════════════════════════════════════

string buildHTTPResponse(const string& body, const string& ct = "application/json") {
    string resp = "HTTP/1.1 200 OK\r\n";
    resp += "Content-Type: " + ct + "; charset=utf-8\r\n";
    resp += "Access-Control-Allow-Origin: *\r\n";   // allow browser fetch
    resp += "Access-Control-Allow-Methods: GET\r\n";
    resp += "Content-Length: " + to_string(body.size()) + "\r\n";
    resp += "Connection: close\r\n\r\n";
    resp += body;
    return resp;
}

void handleRequest(SOCKET client) {
    char buf[4096] = {};
    recv(client, buf, sizeof(buf) - 1, 0);

    string request(buf);
    // Extract: GET /path?query HTTP/1.1
    size_t start = request.find(' ');
    size_t end   = request.find(' ', start + 1);
    if(start == string::npos || end == string::npos) {
        CLOSE_SOCKET(client);
        return;
    }

    string url = request.substr(start + 1, end - start - 1);
    ParsedURL parsed = parseURL(url);
    string& path = parsed.path;
    auto&   p    = parsed.params;

    string body;
    if      (path == "/buildings")    body = handleBuildings();
    else if (path == "/graph")        body = handleGraph();
    else if (path == "/bfs")          body = handleBFS(p);
    else if (path == "/dfs")          body = handleDFS(p);
    else if (path == "/dijkstra")     body = handleDijkstra(p);
    else if (path == "/resources")    body = handleResources(p);
    else if (path == "/allocate")     body = handleAllocate(p);
    else if (path == "/allocate_lab") body = handleAllocateLab(p);
    else if (path == "/deallocate")   body = handleDeallocate(p);
    else if (path == "/nearest")      body = handleNearest(p, false);
    else if (path == "/nearest_lab")  body = handleNearest(p, true);
    else                              body = "{\"error\":\"Unknown endpoint\"}";

    string response = buildHTTPResponse(body);
    send(client, response.c_str(), (int)response.size(), 0);
    CLOSE_SOCKET(client);
}

// ════════════════════════════════════════════════════════════
//  MAIN — start the server
// ════════════════════════════════════════════════════════════
int main() {
    // Build campus
    CampusGraph G = buildCampusGraph();
    ResourceAllocator RA(G);
    RA.loadFromFile();
    G_ptr  = &G;
    RA_ptr = &RA;

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if(server == INVALID_SOCKET) {
        cerr << "Failed to create socket\n";
        return 1;
    }

    // Allow reuse so restart doesn't fail with "address in use"
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(8080);

    if(bind(server, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "Bind failed. Is port 8080 in use?\n";
        return 1;
    }
    listen(server, 10);



    while(true) {
        sockaddr_in clientAddr{};
        socklen_t   clientLen = sizeof(clientAddr);
        SOCKET client = accept(server, (sockaddr*)&clientAddr, &clientLen);
        if(client == INVALID_SOCKET) continue;
        handleRequest(client);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
