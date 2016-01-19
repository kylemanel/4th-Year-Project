#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

/* This function opens a handle and uses it to send the specified post data to the specified url. Support for reusing the same handle for multiple threads/packets is forthcoming.
 *
 * This should also be tested and adapted with websockets.
 *
 * This probably won't compile unless the -L../../extlibs/curl-7.40.0/lib/.libs/ switch is passed.
 * */
int post_send(char *post, char *url, )
{
 CURL *curl;
 CURLcode res;
 curl_global_init(CURL_GLOBAL_ALL);
 curl = curl_easy_init();
 if(curl) {
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
  res = curl_easy_perform(curl); //make the request, res is the return code
  if(res != CURLE_OK) //check for errors
   fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  curl_easy_cleanup(curl);
 }
 curl_global_cleanup();
 return 0;
}

