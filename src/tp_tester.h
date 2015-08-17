#include<string>
#include<iostream>
#include<utility>
#include "json.h"
#include "tp_client.h"


class TPTester {
  using json = nlohmann::json;
  public:
    TPTester(const std::string& url):client(url){}

  inline json request_core(uint node_count, int min_length, int max_length,
                    double max_ratio){
    return client.request_core_json(node_count, min_length, max_length, max_ratio);
  }
  inline json request_bundle(const BoundingBox& bbox, uint core_size, int min_prio,
                      int min_length, int max_length, double max_ratio){
    return client.request_bundle_json(bbox, core_size, min_prio, min_length, max_length, max_ratio);
  }
  private:
    TPClient client;
};
