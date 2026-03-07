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
} resposta;

typedef struct no {
    char * valor;
    struct no* prox;
} no;

void criar_lista(no **l){
    *l = NULL;
}

void insere_fim(no**l, char * valor){
    no * novo = (no*)malloc(sizeof(no));
    if(!novo) exit(1);

    novo->valor = valor;
    novo->prox = NULL;

    if (!*l) {
        *l = novo;
        return;
    }
    no *aux = *l;
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = novo;
}

void limpa_lista(no **l){
    while(*l) {
        no *temp = *l;
        *l = (*l)->prox;
        free(temp->valor);
        free(temp);
    }
}

void imprimir_lista(no *h, char * tipo) {
    no *atual = h;

    printf("\n+-------------------------------------------------------------+\n");
    printf("| RESULTADOS DA EXTRACAO                                      |\n");
    printf("+-------------------------------------------------------------+\n\n");

    while (atual != NULL) {
        if(!strcmp(tipo, "link")) {
            printf("  > [LINK]    https://pt.wikipedia.org%s\n", atual->valor);
        } else if(!strcmp(tipo, "imagens")) {
            printf("  > [IMAGEM]  %s\n", atual->valor);
        } else {
            printf("  > [TOPICO]  %s\n", atual->valor);
        }
        atual = atual->prox;
    }
    printf("\n+--------------------------- FIM -----------------------------+\n");
}
//lógica de lista acima

//função que compila os regex para testar se ele está em conformidade
int comp_regex(regex_t *rewiki, regex_t *relink, regex_t *reimg, regex_t *retop) {
    char padrao_wiki[] = "^https?:\\/\\/pt\\.wikipedia\\.org\\/wiki\\/[[:alnum:]_%()-]+$";
    char padrao_links[]= "href=['\"](\\/wiki\\/[^:#\"']+)['\"]";
    char padrao_imagens[] = "<img[^>]+src=['\"][^'\"]*upload\\.wikimedia\\.org[^'\"]*/([^/'\"?]+\\.(jpg|jpeg|png|svg|webp))[^'\"]*['\"]";
    char padrao_topicos[] = "<h2[^>]*>(<[^>]+>)*([^<]+)</h2>";

    if((regcomp(rewiki, padrao_wiki, REG_EXTENDED) != 0)||
       (regcomp(relink, padrao_links, REG_EXTENDED | REG_ICASE)!=0) ||
       regcomp(reimg, padrao_imagens, REG_EXTENDED | REG_ICASE) != 0 ||
       regcomp(retop, padrao_topicos, REG_EXTENDED) != 0) {
        return 0;
    }
    return 1;
}
//função que controla a opção escolhida entre mostrar links,tópicos e nomes das imagens
void opcao_escolhida(regex_t * reg, char *p, regmatch_t *matches, no **head, int grupo) {
    while ((regexec(reg, p, 4, matches, 0) == 0)) {
        int start = matches[grupo].rm_so;
        int end = matches[grupo].rm_eo;
        int len = end - start;
        char *temp = malloc(len + 1);
        if(temp) {
            memcpy(temp, p + start, len);
            temp[len] = '\0';
            insere_fim(head, temp);
        }
        p += matches[0].rm_eo;
    }
}
//função auxiliar para salvar a requisição http em memória
static size_t save_on_memory(void *ptr, size_t size, size_t nmemb, void *userdata){
    size_t tam_real = nmemb * size;
    resposta *response = (resposta*)userdata;
    char * temp = realloc(response->content, response->tam + tam_real + 1);
    if (!temp) return 0;
    response->content = temp;
    memcpy(&(response->content[response->tam]), ptr, tam_real);
    response->tam += tam_real;
    response->content[response->tam] = 0;
    return tam_real;
}

int main(void){
    CURL *curl = curl_easy_init();
    resposta buffer;
    no *head_lista = NULL;
    buffer.content = malloc(1);
    buffer.tam = 0;
    setlocale(LC_ALL, "");

    regex_t url_wiki, links, imagens, topicos;
    regmatch_t matches[4];
    char url_user[90];

    printf("===============================================================\n");
    printf("                    WebIsCrapper WIKIPEDIA                     \n");
    printf("===============================================================\n");

    if(!comp_regex(&url_wiki, &links, &imagens, &topicos)) return 1;

    printf("\nURL da Wikipedia: ");
    scanf("%89s", url_user);

    if(regexec(&url_wiki, url_user, 0, NULL, 0) == 0) {
        if (curl){
            CURLcode result;
            criar_lista(&head_lista);
            printf("[*] Processando requisicao... Aguarde.\n");

            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bot_aprendizado_web_scrapper/1.0");
            curl_easy_setopt(curl, CURLOPT_URL, url_user);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION , save_on_memory);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);

            result = curl_easy_perform(curl);

            if (result == CURLE_OK){
                int funcao = 0;
                printf("\nMENU DE OPCOES:\n");
                printf("  [1] Listar todos os links internos\n");
                printf("  [2] Listar nomes de arquivos de imagem\n");
                printf("  [3] Listar todos os topicos (H2)\n");
                printf("\nSua escolha: ");
                if(scanf("%d", &funcao) == 1) {
                    char * p = buffer.content;
                    switch (funcao) {
                        case 1:
                            opcao_escolhida(&links, p, matches, &head_lista, 1);
                            imprimir_lista(head_lista, "link");
                            break;
                        case 2:
                            opcao_escolhida(&imagens, p, matches, &head_lista, 1);
                            imprimir_lista(head_lista, "imagens");
                            break;
                        case 3:
                            opcao_escolhida(&topicos, p, matches, &head_lista, 2);
                            imprimir_lista(head_lista, "topicos");
                            break;
                        default:
                            printf("\n[!] Opcao invalida.\n");
                    }
                }
            }
            curl_easy_cleanup(curl);
        }
        free(buffer.content);
        regfree(&url_wiki);
        regfree(&links);
        regfree(&imagens);
        regfree(&topicos);
        limpa_lista(&head_lista);
        return 0;
    } else {
        printf("\n[!] Erro: URL invalida ou fora do padrao Wikipedia PT.\n");
        free(buffer.content);
        regfree(&url_wiki);
        regfree(&links);
        regfree(&imagens);
        regfree(&topicos);
        return 1;
    }
}
