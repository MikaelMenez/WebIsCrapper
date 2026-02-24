#include <stdio.h>
#include <curl/curl.h>
int main(void){
    CURL *curl = curl_easy_init();
    if (curl){
        CURLcode result;
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bot_aprendizado_web_scrapper/1.0 (mixaelmenezes@gmail.com)");
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING,"");
        curl_easy_setopt(curl, CURLOPT_URL, "https://pt.wikipedia.org/wiki/Ben_10");

        result=curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return 0;
}
