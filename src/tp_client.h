#pragma once
#include <curl/curl.h>
#include <string>
#include "types.h"
#include "core.h"
#include "json.h"


class TPClient {
  using json = nlohmann::json;
  friend class TPTester;
 public:
  TPClient(const std::string& base_url) : base_url(base_url), json_header(nullptr) {
    // This is NOT THREAD SAFE so create TPClient at the start of main
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    json_header = curl_slist_append(json_header, "Accept: application/json");
    json_header = curl_slist_append(json_header, "Content-Type: application/json");
  }
  ~TPClient() {
    if (curl) {
      curl_slist_free_all(json_header);
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
  }
  Core request_core(uint node_count, int min_length, int max_length,
                    double max_ratio);
  Draw request_bundle(const BoundingBox& bbox, uint core_size, int min_prio,
                      int min_length, int max_length, double max_ratio);

 private:
  // TODO add error handling
  json post(const std::string& resource, const json& body);
  std::string base_url;
  CURL* curl;
  curl_slist* json_header;
};
