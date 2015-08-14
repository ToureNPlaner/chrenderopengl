#pragma once

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
 * A BoundingBox defined in geographical coordinates
 */
struct BoundingBox {
  float min_longitude;
  float min_latitude;

  float max_longitude;
  float max_latitude;
};
