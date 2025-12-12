#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "dist.h"
#include "graph.h"

using namespace std;

struct BuildingInfo {
  long long id;
  Coordinates location;
  string name;
  string abbr;

  BuildingInfo() : id(0) {}

  BuildingInfo(long long id, Coordinates location, string name, string abbr)
      : id(id), location(location), name(name), abbr(abbr) {}

  bool operator==(const BuildingInfo &other) const {
    return id == other.id && name == other.name && abbr == other.abbr &&
           location.lat == other.location.lat &&
           location.lon == other.location.lon;
  }
};







void buildGraph(istream &input, graph<long long, double> &G,
                vector<BuildingInfo> &buildings,
                unordered_map<long long, Coordinates> &coords);







BuildingInfo getBuildingInfo(const vector<BuildingInfo> &buildings,
                             const string &query);





BuildingInfo getClosestBuilding(const vector<BuildingInfo> &buildings,
                                Coordinates c);









vector<long long> dijkstra(const graph<long long, double> &G, long long start,
                           long long target, const set<long long> &ignoreNodes);


void application(const vector<BuildingInfo> &Buildings,
                 const graph<long long, double> &G);
