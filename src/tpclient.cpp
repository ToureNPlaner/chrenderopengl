#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include <curl/curl.h>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"


typedef std::string bytebuf;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                            void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}


using namespace rapidjson;

int useless_request() {
  CURL* curl;
  CURLcode res;
  bytebuf read_buffer;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://tourenplaner.informatik.uni-stuttgart.de/info");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      std::cerr << "Curl request failed: " << curl_easy_strerror(res) << std::endl;
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
    curl_easy_cleanup(curl);
  }
  return 0;
}


