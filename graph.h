#pragma once

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>



using namespace std;




template <typename VertexT, typename WeightT>
class graph {
 private:
  
  unordered_map<VertexT,unordered_map<VertexT, WeightT>> vibes;
  size_t edgey;



 public:
  
  graph() {
    
    edgey = 0;

  }

  
  
  
  bool addVertex(VertexT v) {
    if (vibes.find(v) != vibes.end()) {

      return false;
    }


    vibes.emplace(v,unordered_map<VertexT, WeightT>{});
    
    return true;
  }

  
  
  
  
  
  
  
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    auto fromy = vibes.find(from);
    auto t = vibes.find(to);





    if (fromy == vibes.end() || t ==vibes.end()) {
      return false;
    }
    auto& bro = fromy->second;



    
    auto l = bro.find(to);
    if (l ==bro.end()) {

      bro.emplace(to, weight);


      edgey++;
    } else {



      l->second = weight;
    }


    
    return true;
  }

  
  
  
  
  
  
  
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {

    auto fromy = vibes.find(from);
    if (fromy ==  vibes.end()) {
      return false;
    }



    auto l = fromy->second.find(to);
    if (l == fromy->second.end()) {
      return false;
    }
    weight = l->second;

    
    return true;
  }

  
  
  
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT> S;
    auto vibeIt = vibes.find(v);
    if (vibeIt == vibes.end()) {
      return S;
    }

    for (auto const& kv :vibeIt->second) {
      S.insert(kv.first);
    }


    
    return S;
  }

  
  vector<VertexT> getVertices() const {
    vector<VertexT> out;
    for (auto const& kv : vibes) {

      out.push_back(kv.first);
    }
    
    return out;
  }

  
  size_t numVertices() const {
    
    return vibes.size();
  }

  
  
  size_t numEdges() const {
    
    return edgey;
  }
};
