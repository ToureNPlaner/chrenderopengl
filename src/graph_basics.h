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

const double EARTH_RADIUS = 6372797.560856;
const double DEG_TO_RAD = 0.017453292519943295769236907684886;

double inline euclidian_distance(const Node& from, const Node& to){
  double latitudeArc  = (from.lat - to.lat) * DEG_TO_RAD;
  double longitudeArc = (from.lon - to.lon) * DEG_TO_RAD;
  double latitudeH = sin(latitudeArc * 0.5);
  latitudeH *= latitudeH;
  double lontitudeH = sin(longitudeArc * 0.5);
  lontitudeH *= lontitudeH;
  double tmp = cos(from.lat*DEG_TO_RAD) * cos(to.lat*DEG_TO_RAD);
  return 2.0 * asin(sqrt(latitudeH + tmp*lontitudeH))*EARTH_RADIUS;
}

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
  Draw() {}
  Draw(const std::vector<Node>& nodes_in, const std::vector<Edge>& edges_in)
      : nodes(nodes_in), edges(edges_in) {
    std::cerr << "Draw Copy Constructor called" << std::endl;
  }
  Draw(std::vector<Node>&& nodes_in, std::vector<Edge>&& edges_in)
      : nodes(std::move(nodes_in)), edges(std::move(edges_in)) {}
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

  BoundingBox():min_longitude(0),min_latitude(0),max_longitude(0),max_latitude(0){}

  bool operator!=(const BoundingBox other){
    const long multiplier = 10000000;
    int diff = 0;
    const int min_lat = min_latitude * multiplier;
    const int min_lat_o = other.min_latitude * multiplier;
    diff += std::abs(min_lat-min_lat_o);
    const int min_lon = min_longitude * multiplier;
    const int min_lon_o = other.min_longitude * multiplier;
    diff += std::abs(min_lon-min_lon_o);
    const int max_lat = max_latitude * multiplier;
    const int max_lat_o = other.max_latitude * multiplier;
    diff += std::abs(max_lat-max_lat_o);
    const int max_lon = max_longitude * multiplier;
    const int max_lon_o = other.max_longitude * multiplier;
    diff += std::abs(max_lon-max_lon_o);
    return diff >= 1000;
  }
};
