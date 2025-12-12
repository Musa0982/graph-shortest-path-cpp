#include "application.h"
#include "json.hpp"

#include <iostream>
#include <limits>
#include <map>
#include <queue> 
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dist.h"
#include "graph.h"

using namespace std;

double INF = numeric_limits<double>::max();

void buildGraph(istream &input, graph<long long, double> &g,
                vector<BuildingInfo> &buildings,
                unordered_map<long long, Coordinates> &coords) {
  
  nlohmann::json root;
  input >> root;

  const auto &buildingsJson = root["buildings"];
  const auto &waypointsJson = root["waypoints"];
  const auto &footwaysJson = root["footways"];


  for (const auto &buildingJson : buildingsJson) {
    long long id = buildingJson["id"].get<long long>();
    double lat = buildingJson["lat"].get<double>();
    double lon = buildingJson["lon"].get<double>();
    string name = buildingJson["name"].get<string>();
    string abbr = buildingJson["abbr"].get<string>();

    Coordinates location(lat, lon);

    g.addVertex(id);
    buildings.push_back(BuildingInfo(id, location, name, abbr));
  }

  for (const auto &waypointJson : waypointsJson) {
    long long id = waypointJson["id"].get<long long>();
    double lat = waypointJson["lat"].get<double>();
    double lon = waypointJson["lon"].get<double>();

    Coordinates location(lat, lon);

    g.addVertex(id);
    coords[id] = location;
  }

  for (const auto &footwayJson : footwaysJson) {
    if (footwayJson.size() < 2) {
      continue;
    }

    for (size_t i = 0; i + 1 < footwayJson.size(); ++i) {
      long long fromId = footwayJson[i].get<long long>();
      long long toId = footwayJson[i + 1].get<long long>();
      auto itFrom = coords.find(fromId);

      auto itTo = coords.find(toId);
      if (itFrom == coords.end() || itTo == coords.end()) {
        continue;
      }

      double distance = distBetween2Points(itFrom->second, itTo->second);

      g.addEdge(fromId, toId, distance);
      g.addEdge(toId, fromId, distance);
    }
  }

  for (const auto &buildingInfo : buildings) {
    for (const auto &entry : coords) {
      long long waypointId = entry.first;
      const Coordinates &waypointLocation = entry.second;

      double distance =
          distBetween2Points(buildingInfo.location, waypointLocation);

      if (distance <= 0.036) {
        
        g.addEdge(buildingInfo.id, waypointId, distance);
        g.addEdge(waypointId, buildingInfo.id, distance);
      }
    }
  }

}

BuildingInfo getBuildingInfo(const vector<BuildingInfo> &buildings,
                             const string &query) {
  for (const BuildingInfo &building : buildings) {
    if (building.abbr == query) {
      return building;
    } else if (building.name.find(query) != string::npos) {
      return building;
    }
  }
  BuildingInfo fail;
  fail.id = -1;
  return fail;
}

BuildingInfo getClosestBuilding(const vector<BuildingInfo> &buildings,
                                Coordinates c) {
  double minDestDist = INF;
  BuildingInfo ret = buildings.at(0);
  for (const BuildingInfo &building : buildings) {
    double dist = distBetween2Points(building.location, c);
    if (dist < minDestDist) {
      minDestDist = dist;
      ret = building;
    }
  }
  return ret;
}

vector<long long> dijkstra(const graph<long long, double> &G, long long start,
                           long long target,
                           const set<long long> &ignoreNodes) {


  if (start == target) {

    vector<long long> only;
    only.push_back(start);
    return only;
  }
  vector<long long> verts = G.getVertices();
  bool hasStart = false;
  bool hasTarget = false;






  for (long long v : verts) {
    if (v == start) {
      hasStart = true;
    }
    if (v == target) {
      hasTarget = true;
    }
  }
  if (!hasStart || !hasTarget) {
    return vector<long long>();
  }




  unordered_map<long long,double> bestDist;
  unordered_map<long long, long long>prevHop;
  for (long long v : verts) {
    bestDist[v] = INF;
  }
  bestDist[start] = 0.0;

  class prioritize {
  public:
    bool operator()(const pair<long long, double> &p1,
                    const pair<long long, double> &p2) const {
      return p1.second > p2.second;
    }
  };



  priority_queue<pair<long long, double>,
                 vector<pair<long long, double>>,
                 prioritize>
      worklist;

  worklist.push({start, 0.0});
  while (!worklist.empty()) {

    pair<long long,double> curItem = worklist.top();
    worklist.pop();
    long long curNode = curItem.first;
    double curDist = curItem.second;



    if (curDist >bestDist[curNode]) {
      continue;
    }
    if (curNode ==  target) {
      break;
    }
    set<long long> pals = G.neighbors(curNode);
    for (long long nextNode : pals) {


      if (ignoreNodes.count(nextNode) > 0 && nextNode != start && nextNode != target) {
        continue;
      }
      double edgeW = 0.0;
      if (!G.getWeight(curNode, nextNode, edgeW)) {
        continue;

      }


      double newDist = curDist +  edgeW;
      if (newDist <bestDist[nextNode]) {
        bestDist[nextNode] = newDist;
        prevHop[nextNode] =curNode;
        worklist.push({nextNode, newDist});
      }
    }
  }
  if (bestDist[target]==INF) {
    return vector<long long>();
  }


  stack<long long> st;
  long long walk = target;
  st.push(walk);
  while (walk != start) {
    if (prevHop.find(walk) == prevHop.end()) {
      return vector<long long>();
    }
    walk = prevHop[walk];
    st.push(walk);
  }



  vector<long long> path;
  while (!st.empty()) {
    path.push_back(st.top());
    st.pop();
  }



  
  return path;
  
}

double pathLength(const graph<long long, double> &G,
                  const vector<long long> &path) {
  double length = 0.0;
  double weight;
  for (size_t i = 0; i + 1 < path.size(); i++) {
    bool res = G.getWeight(path.at(i), path.at(i + 1), weight);
    if (!res) {
      return -1;
    }
    length += weight;
  }
  return length;
}

void outputPath(const vector<long long> &path) {
  for (size_t i = 0; i < path.size(); i++) {
    cout << path.at(i);
    if (i != path.size() - 1) {
      cout << "->";
    }
  }
  cout << endl;
}


void application(const vector<BuildingInfo> &buildings,
                 const graph<long long, double> &G) {
  string person1Building, person2Building;

  set<long long> buildingNodes;
  for (const auto &building : buildings) {
    buildingNodes.insert(building.id);
  }

  cout << endl;
  cout << "Enter person 1's building (partial name or abbreviation), or #> ";
  getline(cin, person1Building);

  while (person1Building != "#") {
    cout << "Enter person 2's building (partial name or abbreviation)> ";
    getline(cin, person2Building);

    
    BuildingInfo p1 = getBuildingInfo(buildings, person1Building);
    BuildingInfo p2 = getBuildingInfo(buildings, person2Building);
    Coordinates P1Coords, P2Coords;
    string P1Name, P2Name;

    if (p1.id == -1) {
      cout << "Person 1's building not found" << endl;
    } else if (p2.id == -1) {
      cout << "Person 2's building not found" << endl;
    } else {
      cout << endl;
      cout << "Person 1's point:" << endl;
      cout << " " << p1.name << endl;
      cout << " " << p1.id << endl;
      cout << " (" << p1.location.lat << ", " << p1.location.lon << ")" << endl;
      cout << "Person 2's point:" << endl;
      cout << " " << p2.name << endl;
      cout << " " << p2.id << endl;
      cout << " (" << p2.location.lon << ", " << p2.location.lon << ")" << endl;

      Coordinates centerCoords = centerBetween2Points(p1.location, p2.location);
      BuildingInfo dest = getClosestBuilding(buildings, centerCoords);

      cout << "Destination Building:" << endl;
      cout << " " << dest.name << endl;
      cout << " " << dest.id << endl;
      cout << " (" << dest.location.lat << ", " << dest.location.lon << ")"
           << endl;

      vector<long long> P1Path = dijkstra(G, p1.id, dest.id, buildingNodes);
      vector<long long> P2Path = dijkstra(G, p2.id, dest.id, buildingNodes);

      
      if (P1Path.empty() || P2Path.empty()) {
        cout << endl;
        cout << "At least one person was unable to reach the destination "
                "building. Is an edge missing?"
             << endl;
        cout << endl;
      } else {
        cout << endl;
        cout << "Person 1's distance to dest: " << pathLength(G, P1Path);
        cout << " miles" << endl;
        cout << "Path: ";
        outputPath(P1Path);
        cout << endl;
        cout << "Person 2's distance to dest: " << pathLength(G, P2Path);
        cout << " miles" << endl;
        cout << "Path: ";
        outputPath(P2Path);
      }
    }

    
    
    
    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);
  }
}
