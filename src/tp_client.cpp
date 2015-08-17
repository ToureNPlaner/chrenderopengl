#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <utility>
#include <cstring>
#include "tp_client.h"

using json = nlohmann::json;

static size_t string_write_callback(void* contents, size_t size, size_t nmemb,
                                    void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

struct string_read_helper {
  string_read_helper(std::string&& in) : s(std::move(in)), pos(0) {}
  string_read_helper(const std::string& in) : s(in), pos(0) {}
  const std::string s;
  size_t pos;
};

static size_t string_read_callback(void* ptr, size_t size, size_t nmemb,
                                   void* userp) {
  string_read_helper* srh = static_cast<string_read_helper*>(userp);
  size_t bytes_wanted = size * nmemb;

  if (bytes_wanted < 1 || srh->s.size() < 1) {
    return 0;
  }
  size_t bytes_available = srh->s.size() - srh->pos;
  size_t send_bytes = std::min(bytes_available, bytes_wanted);
  std::memcpy(ptr, srh->s.data() + srh->pos, send_bytes);
  srh->pos += send_bytes;
  return send_bytes;
}

json TPClient::post(const std::string& resource, const json& body) {
  CURLcode res;
  std::string read_buffer;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, (base_url + resource).c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    // Set header
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, json_header);
    string_read_helper srh(body.dump());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, string_read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &srh);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, srh.s.size());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, string_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "Curl request failed: " << curl_easy_strerror(res)
                << std::endl;
      return read_buffer;
    }
  }
  // No copy here since RVO or move
  return json::parse(std::move(read_buffer));
}


Core TPClient::request_core(uint node_count, int min_length, int max_length,
                            double max_ratio) {
  json body = {{"nodeCount", node_count},
               {"minLen", min_length},
               {"maxLen", max_length},
               {"maxRatio", max_ratio},
               {"coords", "latlon"}};
  json res =  post("/algdrawcore", body);
  Draw draw;
  auto vertices = res["draw"]["vertices"];
  const double divider = 10000000;
  for (auto it = vertices.begin(); it != vertices.end();) {
    double lat = (double)*it++ / divider;
    double lon = (double)*it++ / divider;
    draw.nodes.emplace_back(Node(lat, lon));
  }

  auto draw_edges = res["draw"]["edges"];
  for (auto it = draw_edges.begin(); it != draw_edges.end();) {
    uint src = *it++;
    uint trgt = *it++;
    uint type = *it++;
    draw.edges.emplace_back(Edge(src, trgt, type/2, type));
  }

  return Core(std::move(draw));
}

Draw TPClient::request_bundle(const BoundingBox& bbox, uint core_size, int min_prio, int min_length, int max_length, double max_ratio) {
  const long multiplier = 10000000;
  const int node_count = 800;
  const int lat_min = bbox.min_latitude*multiplier;
  const int lon_min = bbox.min_longitude*multiplier;
  const int lat_max = bbox.max_latitude*multiplier;
  const int lon_max = bbox.max_longitude*multiplier;
  const int width = lat_max-lat_min;
  const int height = lon_max-lon_min;
  //lat 478632064lon 82457144width 9277472height 24863696
  //std::cout << "lat " << lat_min << " lon " << lon_min << " width " << width << " height " << height <<std::endl;
  json body = {
    {"bbox", 
      {
        {"x", lat_min},
        {"y", lon_min},
        {"width", width},
        {"height", height},
      }
    },

    {"level", min_prio},
    {"nodeCount", node_count},
    {"coreSize", core_size},
    {"mode", "auto"},
    {"minLen", min_length},
    {"maxLen", max_length},
    {"maxRatio", max_ratio},
    {"coords", "latlon"}
  };
  json res = post("/algbbbundle", body);
  Draw draw;
  auto vertices = res["draw"]["vertices"];
  const double divider = 10000000;
  for(auto it = vertices.begin(); it != vertices.end();){
    double lat = (double) *it++/divider;
    double lon = (double) *it++/divider;
    draw.nodes.emplace_back(Node(lat, lon));
  }

  auto draw_edges = res["draw"]["edges"];
  for(auto it = draw_edges.begin(); it != draw_edges.end();){
    uint src = *it++;
    uint trgt = *it++;
    uint type = *it++;
    draw.edges.emplace_back(Edge(src, trgt, type/2, type));
  }


  return draw;
}

