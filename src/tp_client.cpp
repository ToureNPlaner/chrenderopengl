#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include "tp_client.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

typedef std::string bytebuf;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                            void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

using namespace rapidjson;

int TPClient::useless_request() {
  CURLcode res;
  bytebuf read_buffer;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, (base_url + "/info").c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
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
