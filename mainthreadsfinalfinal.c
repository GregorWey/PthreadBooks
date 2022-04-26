#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;

typedef struct dados
{
    int codigo;
    char titulo[100];
    char nome[100];
    int edicao;
    int ano;
} dados;

typedef struct nodo
{
    int ano;
    struct nodo *prox;
} elemento;

typedef struct anos
{
    int ano;
    int qtd;
    struct anos *esq;
    struct anos *dir;
    struct anos *pai;
} elementoanos;

typedef struct autor
{
    char nome[100];
    int qtd;
    struct autor *esq;
    struct autor *dir;
    struct autor *pai;
} elementoautor;

typedef struct livro
{
    char titulo[100];
    struct nodo *anos;
    struct livro *esq;
    struct livro *dir;
    struct livro *pai;
} elementolivro;

struct anos *raizano;
struct autor *raizautor;
struct livro *raizlivro;
int maisano[2]={-1,-1};// pos0 = ano pos1=qtd de livros

int totalanos;
int totallivros;
void limpaautores(elementoautor *raiz)
{
     if (raiz == NULL)
    {
        return;
    }
    limpaautores(raiz->esq);
    limpaautores(raiz->dir);
    free(raiz);
}

void limpaedi(elementoanos *raiz)
{
    if (raiz == NULL)
    {
        return;
    }
    limpaedi(raiz->esq);
    limpaedi(raiz->dir);
    free(raiz);
}

void limpalivros(elementolivro *raiz)
{
    if (raiz == NULL)
    {
        return;
    }
    limpalivros(raiz->esq);
    limpalivros(raiz->dir);
    free(raiz);
}

void inserenodoedi(struct anos *atual, struct anos *novo)
{
    if (novo->ano == atual->ano)
    {
        atual->qtd++;
    }
    else if (novo->ano < atual->ano)
    {
        if (atual->esq == NULL)
        {
            atual->esq = novo;
            atual->esq->pai = atual;
        }
        else
        {
            inserenodoedi(atual->esq, novo);
        }
    }
    else
    {
        if (atual->dir == NULL)
        {
            atual->dir = novo;
            atual->dir->pai = atual;
        }
        else
        {
            inserenodoedi(atual->dir, novo);
        }
    }
}

void inserenodoaut(struct autor *atual, struct autor *novo)
{
    if (strcmp(atual->nome, novo->nome) == 0)
    {
        atual->qtd++;
    }
    else if (strcmp(novo->nome, atual->nome) < 0)
    {
        if (atual->esq == NULL)
        {
            atual->esq = novo;
            atual->esq->pai = atual;
        }
        else
        {
            inserenodoaut(atual->esq, novo);
        }
    }
    else
    {
        if (atual->dir == NULL)
        {
            atual->dir = novo;
            atual->dir->pai = atual;
        }
        else
        {
            inserenodoaut(atual->dir, novo);
        }
    }
}

void inserenodolivro(struct livro *atual, struct livro *novo)
{
    if (strcmp(atual->titulo, novo->titulo) == 0)
    {
        struct nodo *pnovo;
        struct nodo *anosatuais;
        pnovo = (struct nodo *)malloc(sizeof(elemento));
        anosatuais = (struct nodo *)malloc(sizeof(elemento));
        pnovo->ano = novo->anos->ano;
        pnovo->prox = NULL;

        anosatuais = atual->anos;
        
        while(anosatuais != NULL)
        {
             if(anosatuais->ano == pnovo->ano)
                return;
            
            anosatuais = anosatuais->prox;
        }
        pnovo->prox = atual->anos;
        atual->anos = pnovo;
    }
    else if (strcmp(novo->titulo, atual->titulo) < 0)
    {
        if (atual->esq == NULL)
        {
            atual->esq = novo;
            atual->esq->pai = atual;
            totallivros++;
        }
        else
        {
            inserenodolivro(atual->esq, novo);
        }
    }
    else
    {
        if (atual->dir == NULL)
        {
            atual->dir = novo;
            atual->dir->pai = atual;
            totallivros++;
        }
        else
        {
            inserenodolivro(atual->dir, novo);
        }
    }
}

void *autored(void *args)
{
    dados *d = (dados *)args;

    struct autor *novo;
    novo = (struct autor *)malloc(sizeof(elementoautor));

    pthread_mutex_lock(&mutex);
    strcpy(novo->nome, d->nome);
    pthread_mutex_unlock(&mutex);

    novo->qtd = 1;
    novo->dir = NULL;
    novo->esq = NULL;
    novo->pai = NULL;
    if (raizautor == NULL)
    {
        raizautor = novo;
    }
    else
    {
        inserenodoaut(raizautor, novo);
    }
}

void *ediano(void *args)
{
    dados *d = (dados *)args;

    struct anos *novo;
    novo = (struct anos *)malloc(sizeof(elementoanos));

    pthread_mutex_lock(&mutex);
    novo->ano = d->ano;
    pthread_mutex_unlock(&mutex);

    novo->qtd = 1;
    novo->esq = NULL;
    novo->dir = NULL;
    novo->pai = NULL;
    if (raizano == NULL)
    {
        raizano = novo;
    }
    else
    {
        inserenodoedi(raizano, novo);
    }
}

void *livroano(void *args)
{
    dados *d = (dados *)args;

    struct livro *novo;
    novo = (struct livro *)malloc(sizeof(elementolivro));
    novo->anos = (struct nodo *)malloc(sizeof(elemento));

    pthread_mutex_lock(&mutex);
    strcpy(novo->titulo, d->titulo);
    novo->anos->ano = d->ano;
    pthread_mutex_unlock(&mutex);

    novo->anos->prox = NULL;
    novo->dir = NULL;
    novo->esq = NULL;
    novo->pai = NULL;
    if (raizlivro == NULL)
    {
        raizlivro = novo;
    }
    else
    {
        inserenodolivro(raizlivro, novo);
    }
}

void mostraEdi(struct anos *raiz)
{
    if (raiz == NULL)
    {
        return;
    }

    mostraEdi(raiz->esq);
    FILE *eds;
    eds = fopen("edicao.txt", "a+");
    if(raiz->qtd > maisano[1]){
        maisano[0]=raiz->ano;
        maisano[1]=raiz->qtd;
    }
    fprintf(eds, "%d | %d \n", raiz->ano, raiz->qtd);
    fclose(eds);
    totalanos++;
    mostraEdi(raiz->dir);
}

void mostraAutor(struct autor *raiz)
{
    if (raiz == NULL)
    {
        return;
    }

    mostraAutor(raiz->esq);
    FILE *auts;
    auts = fopen("autores.txt", "a+");
    fprintf(auts, "%s | %d \n", raiz->nome, raiz->qtd);
    fclose(auts);
    mostraAutor(raiz->dir);
}

void mostraLivro(struct livro *raiz)
{
    if (raiz == NULL)
    {
        return;
    }

    mostraLivro(raiz->esq);
    FILE *livs;
    livs = fopen("livro.txt", "a+");
    fprintf(livs, "%s - ", raiz->titulo);
    while (raiz->anos != NULL)
    {
        fprintf(livs, "%d|", raiz->anos->ano);
        raiz->anos = raiz->anos->prox;
    }
    fprintf(livs, "\n");
    fclose(livs);
    mostraLivro(raiz->dir);
}

void *processo_edicao()
{
    mostraEdi(raizano);
}

void *processo_autor()
{
    mostraAutor(raizautor);
}

void *processo_livro()
{
    mostraLivro(raizlivro);
}

void central()
{
    pthread_t idlivro, idedicao, idautor;

    pthread_create(&idlivro, NULL, processo_livro, NULL);
    pthread_create(&idedicao, NULL, processo_edicao, NULL);
    pthread_create(&idautor, NULL, processo_autor, NULL);

    pthread_join(idlivro, NULL);
    pthread_join(idedicao, NULL);
    pthread_join(idautor, NULL);
}

void leituralivros(char *nome)
{
    totalanos=0;
    totallivros=0;
    pthread_t idlivro, idedicao, idautor;
    FILE *livros;
    dados d;
    char linha[400];
    int campo = 0;
    int j;

    livros = fopen(nome, "r");
    if (livros == NULL)
    {
        printf("Arquivo nao encontrado \n");
        exit(1);
    }
    else
    {
        while (fgets(linha, sizeof(linha), livros))
        {
            campo = 0;
            char *field = strtok(linha, ";");
            while (field)
            {
                if (campo == 0)
                {
                    d.codigo = atoi(field);
                }
                if (campo == 1)
                {
                    strcpy(d.titulo, field);
                }
                if (campo == 2)
                {
                    strcpy(d.nome, field);
                }
                if (campo == 3)
                {
                    d.edicao = atoi(field);
                }
                if (campo == 4)
                {
                    d.ano = atoi(field);
                }
                field = strtok(NULL, ";");
                campo++;
            }

            pthread_mutex_init(&mutex, NULL);
            pthread_create(&idlivro, NULL, livroano, (void *)&d);
            pthread_create(&idautor, NULL, autored, (void *)&d);
            pthread_create(&idedicao, NULL, ediano, (void *)&d);

            pthread_join(idlivro, NULL);
            pthread_join(idautor, NULL);
            pthread_join(idedicao, NULL);

            
        }
        fclose(livros);
    }
}

int main()
{
    char nome[20] = "livros2.txt";
    leituralivros(nome);
    central();
    printf("Quantidade total de livros publicados: %d \n",totallivros);
    printf("Quantidade total de anos que houveram publicações: %d \n",totalanos);
    printf("Ano %d teve %d livros publicados.\n",maisano[0],maisano[1]);
    return 0;
}
