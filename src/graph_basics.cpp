#include <utility>
#include "graph_basics.h"

double euclidian_distance(const Node& from, const Node& to) {
  double latitudeArc = (from.lat - to.lat) * DEG_TO_RAD;
  double longitudeArc = (from.lon - to.lon) * DEG_TO_RAD;
  double latitudeH = sin(latitudeArc * 0.5);
  latitudeH *= latitudeH;
  double lontitudeH = sin(longitudeArc * 0.5);
  lontitudeH *= lontitudeH;
  double tmp = cos(from.lat * DEG_TO_RAD) * cos(to.lat * DEG_TO_RAD);
  return 2.0 * asin(sqrt(latitudeH + tmp * lontitudeH)) * EARTH_RADIUS;
}
