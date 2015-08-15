#pragma once
#include <curl/curl.h>
#include <string>
#include "types.h"
#include "graph_basics.h"
#include "json.h"

using json = nlohmann::json;

class TPClient {
 public:
  TPClient(std::string& base_url)
      : base_url(base_url), curl(curl_easy_init()) {}
  TPClient(std::string&& base_url)
      : base_url(std::move(base_url)), curl(curl_easy_init()) {}
  ~TPClient() {
    if (curl) {
      curl_easy_cleanup(curl);
    }
  }
  void request_core(uint node_count, int min_length, int max_length,
                    double max_ratio);
  void request_bundle(BoundingBox& bbox, uint core_size, int min_prio,
                      int min_length, int max_length, double max_ratio);

 private:
  // TODO add error handling
  json post(const std::string& resource, const json& body);
  std::string base_url;
  CURL* curl;
};
