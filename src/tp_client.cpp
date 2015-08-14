#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cstring>
#include "tp_client.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"


static size_t string_write_callback(void* contents, size_t size, size_t nmemb,
                            void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

struct string_read_helper {
  string_read_helper(const std::string& in):s(in), pos(0){}
  const std::string& s;
  size_t pos;
};

static size_t string_read_callback(void* ptr, size_t size, size_t nmemb, void* userp)
{
  string_read_helper* srh = static_cast<string_read_helper*>(userp);
  size_t bytes_wanted = size*nmemb;

  if(bytes_wanted < 1 || srh->s.size() < 1){
    return 0;
  }
  size_t bytes_available = srh->s.size() - srh->pos;
  size_t send_bytes = std::min(bytes_available, bytes_wanted);
  std::memcpy(ptr, srh->s.data()+srh->pos, send_bytes);
  srh->pos += send_bytes;
  return send_bytes;
}

std::string TPClient::post(const std::string& resource, const std::string& body) {
  CURLcode res;
  std::string read_buffer;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, (base_url + resource).c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    // Set header
    struct curl_slist* list = nullptr;
    list = curl_slist_append(list, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    string_read_helper srh(body);
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
  // No copy here since RVO or move holds
  return read_buffer;
}

using namespace rapidjson;

void TPClient::request_core(uint node_count, int min_length, int max_length, double max_ratio) {
  std::string body("{\"nodeCount\": 10,\"minLen\": 40,\"maxLen\": 400,\"maxRatio\": 0.01, \"coords\" : \"latlon\"}");
  std::string res = post("/algdrawcore", body);
  std::cerr << res << std::endl;
  return;
}

int TPClient::useless_request() {
  CURLcode res;
  std::string read_buffer;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, (base_url + "/info").c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, string_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "Curl request failed: " << curl_easy_strerror(res)
                << std::endl;
      return 1;
    }

    Document d;
    if (!d.Parse(read_buffer.c_str()).HasParseError()) {
      Value& algorithms = d["algorithms"];
      for (SizeType i = 0; i < algorithms.Size(); ++i) {
        const Value& name = algorithms[i]["name"];
        std::cout << "Name: " << name.GetString() << std::endl;
      }
    }
  }
  return 0;
}
