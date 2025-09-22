#include <string>
#include <iostream>
#include <iomanip>
#include "FetchTask.h"
#include "SongBookApp.h"
#include <curl/curl.h>


const char* api = "http://127.0.0.1:8000";
const char* sapi = "https://www.youtube.com/";

size_t curl_writedata_callback(
                               /** Data received from a server (void* has size of 8 bits)*/
                               void* contents,
                               /** always equals 1 according to docs */
                               size_t size, 
                               /** size of data (contents)*/
                               size_t nmemb, 
                               /** A ptr to a struct that function uses to store received data.
                                *  Assign to this argument is via *CURLOPT_WRITEDATA* opt.
                                */
                               void* userp) {

  for (int i=0; i < size; i++)  {
    // TODO
  }                              
  std::cout << contents << std::endl;
  std::cout << sizeof(contents) << std::endl;

  return 0;
  // TODO
}


// BIG TODO
int FetchTask::startInteractive()
{

  CURL* handle = curl_easy_init();

  // handle setup
  curl_easy_setopt(handle, CURLOPT_URL, sapi);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_writedata_callback);

  CURLcode success = curl_easy_perform(handle);

  switch (success)
  {
  case CURLE_OK:
    std::cout << "TO BE IMPLEMENTED (but request was ok)." << std::endl;
    break;
  case CURLE_URL_MALFORMAT:
    std::cout << "The URL was not propelry formatted! Cannot perform CURL request..." << std::endl;
    break;  
  case CURLE_WRITE_ERROR:
    std:: cout << "The URL write callback error" << std::endl;
    break;
  default:
    std::cout << "Unexpected error code happened (" << success << ")" << std::endl;
    break;
  }

  curl_easy_cleanup(handle);
  return 1;
}
