#include <stddef.h>
#include <stdio.h>
#include<regex.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
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
    setlocale(LC_ALL, ""); //Locale para permitir acentos
    //Iniciando compilacao da regex e pegada da url do user. Tambem verificando match.
    regex_t url_wiki;
    char url_user[90];
    char padraoDaWiki[] = "^https?:\\/\\/[a-z]{2}\\.wikipedia\\.org\\/wiki\\/[[:alnum:]_%]+$";

    if(regcomp(&url_wiki, padraoDaWiki, REG_EXTENDED) != 0)
    {
        printf("Nao foi possivel compilar a regex\n");
        return 1;
    }

    printf("Copie aqui a URL da Wikipedia que voce deseja acessar: ");
    scanf("%s", url_user);

    if(regexec(&url_wiki, url_user, 0, NULL, 0) == 0)
    {
        if (curl){
            CURLcode result;

            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bot_aprendizado_web_scrapper/1.0 (mixaelmenezes@gmail.com)");
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING,"");
            curl_easy_setopt(curl, CURLOPT_URL, url_user);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION , save_on_memory);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);
            result=curl_easy_perform(curl);
            if (result==CURLE_OK){
                printf("esse é o html :%s",buffer.content);
            }
            curl_easy_cleanup(curl);

        }
        free(buffer.content);
        regfree(&url_wiki);
        return 0;
    }
    else
    {
        printf("Texto invalido\n");
        regfree(&url_wiki);
        return 1;
    }
}
