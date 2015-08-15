#pragma once
#include <utility>
#include <vector>

/**
 * Node struct for parsing preprocessed osm graph from file
 */
struct Node {
  Node() : lat(0), lon(0) {}
  Node(double la, double lo) : lat(la), lon(lo) {}

  double lat;
  double lon;
};

/**
 * Edge struct for parsing preprocessed osm graph from file
 */
struct Edge {
  Edge() : source(0), target(0), width(0), color(0) {}
  Edge(uint s, uint t, uint w, uint c)
      : source(s), target(t), width(w), color(c) {}

  uint source;
  uint target;
  uint width;
  int color;
};

/**
 * Holds graph data for drawing
 */
struct Draw {
  Draw(){}
  Draw(const std::vector<Node>& nodes_in,const std::vector<Edge>& edges_in):nodes(nodes_in),edges(edges_in){}
  Draw(std::vector<Node>&& nodes_in,std::vector<Edge>&& edges_in):nodes(std::move(nodes_in)),edges(std::move(edges_in)){
    std::cerr << "Move constructed Draw" << std::endl;
  }
  std::vector<Node> nodes;
  std::vector<Edge> edges;
};


/**
 * A BoundingBox defined in geographical coordinates
 */
struct BoundingBox {
  float min_longitude;
  float min_latitude;

  float max_longitude;
  float max_latitude;
};
