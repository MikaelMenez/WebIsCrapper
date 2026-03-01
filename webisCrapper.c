#include <stddef.h>
#include <stdio.h>
#include <regex.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct {
    char * content;
    size_t tam;
}resposta ;

typedef struct no {
    char * dados;
    struct no* prox;
}no;

typedef struct{
   int tam;
   no* prox;
} head;

head* criar_lista(){
    head*p=malloc(sizeof(head));

    if (!p) {
        return NULL;
    }

    (*p).prox = NULL;
    (*p).tam=0;

    return p;
}
void adicionar_lista(head * head,char * valor){
    no *p=malloc(sizeof(no));

    if (!p) {
        return;
    }

    p->dados=strdup(valor);
    p->prox=head->prox;
    head->prox=p;
    head->tam++;

}
void imprimir_lista(head *h,char * tipo) { //Ver como mudar esse "Link: " pra imagem: ou topico: !!!!!!
    no *atual = h->prox;

    printf("tamanho: %d\n", h->tam);
    while (atual != NULL) {
        printf("%s: %s\n", tipo,atual->dados);
        atual = atual->prox;
    }
}

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
    regex_t links;
    regex_t imagens;
    regex_t topicos;
    regmatch_t matches[4];
    char url_user[90];
    char padraoDaWiki[] = "^https?:\\/\\/pt\\.wikipedia\\.org\\/wiki\\/[[:alnum:]_%]+$";
    char padrao_links[]= "href=\"(\\https:\\/[^#][^\"]*)\"";
    char padrao_imagens[] = "<img[^>]+src=\"[^\\\"]*upload\\.wikimedia\\.org[^\\\"]*/([^/\\\"]+\\.(jpg|jpeg|png|svg|webp))\"";
    char padrao_topicos[] = "<h2[^>]*>(<[^>]+>)*([^<]+)</h2>";

    if((regcomp(&url_wiki, padraoDaWiki, REG_EXTENDED) != 0)||(regcomp(&links, padrao_links, REG_EXTENDED)!=0) || regcomp(&imagens, padrao_imagens, REG_EXTENDED) != 0 || regcomp(&topicos, padrao_topicos, REG_EXTENDED) != 0)
    {
        printf("Nao foi possivel compilar a regex\n");
        return 1;
    }

    printf("Copie aqui a URL da Wikipedia que voce deseja acessar: ");
    scanf("%89s", url_user);

    if(regexec(&url_wiki, url_user, 0, NULL, 0) == 0)
    {
        if (curl){
            CURLcode result;
            head *head=criar_lista();
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bot_aprendizado_web_scrapper/1.0 (mixaelmenezes@gmail.com)");
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING,"");
            curl_easy_setopt(curl, CURLOPT_URL, url_user);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION , save_on_memory);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);
            result=curl_easy_perform(curl);
            if (result==CURLE_OK){
                int funcao=0;

                puts("o que deseja fazer:\ndigite 1: para ver todos os links\ndigite 2: para ver o nome de todas imagens\ndigite 3: para ver todos os topicos do indice do artigo");
                scanf("%d",&funcao);

                char * p;
                p=buffer.content;
                switch (funcao) {
                    case 1:
                    while ((regexec(&links, p, 2, matches, 0) == 0)) {
                        int start=matches[1].rm_so;
                        int end=matches[1].rm_eo;
                        int len=end-start;

                        char *temp=malloc(len+1);
                        memcpy(temp, p+start, len);
                        temp[len]='\0';

                        adicionar_lista(head, temp);

                        free(temp);

                        p += matches[0].rm_eo;
                    }
                    imprimir_lista(head,"link");
                    break;
                    case 2:
                    while (regexec(&imagens, p, 3, matches, 0) == 0)
                    {
                        int start = matches[1].rm_so;
                        int end   = matches[1].rm_eo;
                        int len   = end - start;

                        char *temp = malloc(len + 1);
                        memcpy(temp, p + start, len);
                        temp[len] = '\0';

                        adicionar_lista(head, temp);

                        free(temp);

                        p += matches[0].rm_eo;
                    }
                    imprimir_lista(head,"imagem");
                    break;
                    case 3:
                    while (regexec(&topicos, p, 3, matches, 0) == 0)
                    {
                        int start = matches[2].rm_so;
                        int end   = matches[2].rm_eo;
                        int len   = end - start;

                        char *temp = malloc(len + 1);
                        memcpy(temp, p + start, len);
                        temp[len] = '\0';

                        adicionar_lista(head, temp);
                        free(temp);

                        p += matches[0].rm_eo;
                    }
                    imprimir_lista(head,"tópico");
                    break;
                    default:
                    printf("Essa opção não existe.");
                }

            }
            curl_easy_cleanup(curl);

        }
        free(buffer.content);
        regfree(&url_wiki);
        regfree(&links);
        regfree(&imagens);
        regfree(&topicos);

        return 0;
    }
    else
    {
        printf("Texto invalido\n");
        regfree(&url_wiki);
        return 1;
    }
}
