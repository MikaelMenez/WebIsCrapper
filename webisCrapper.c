#include <stddef.h>
#include <stdio.h>
#include<regex.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    char * content;
    size_t tam;
}resposta ;
static size_t save_on_memory(void *ptr,size_t size,size_t nmemb,void *userdata){
    size_t tam_real=nmemb*size;
    resposta *response=(resposta*)userdata;
    char * temp= realloc(response->content, response->tam+tam_real+1);
    if (!temp) return 0;
    response->content=temp;
    memcpy(&(response->content[response->tam]), ptr, tam_real);
    response->tam+=tam_real;
    response->content[response->tam]=0;
    return tam_real;
}
int main(void){
    CURL *curl = curl_easy_init();
    resposta buffer;
    buffer.content=malloc(1);
    buffer.tam=0;
    if (curl){
        CURLcode result;

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bot_aprendizado_web_scrapper/1.0 (mixaelmenezes@gmail.com)");
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING,"");
        curl_easy_setopt(curl, CURLOPT_URL, "https://pt.wikipedia.org/wiki/Ben_10");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION , save_on_memory);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);
        result=curl_easy_perform(curl);
        if (result==CURLE_OK){
            printf("esse é o html :%s",buffer.content);
        }
        curl_easy_cleanup(curl);

    }
    free(buffer.content);
    return 0;
}
