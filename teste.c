#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
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
void imprimir_lista(head *h) {
    no *atual = h->prox;
    printf("tamanho: %d\n", h->tam);
    while (atual != NULL) {
        printf("Link: %s\n", atual->dados);
        atual = atual->prox;
    }
}
int main(){
    head*head=criar_lista();
    char teste[2][2]={"a\0","b\0"};
    adicionar_lista(head, *teste);
    adicionar_lista(head, *(teste+1));
    imprimir_lista(head);
    return 0;


}
