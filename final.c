#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Disco
typedef struct {
    int tam;
} Disco;

// Torre
typedef struct {
    char nome;
    Disco* pilha[10];
    int topo;
} Torre;

// Registro de histórico
typedef struct {
    char jogador[50];
    int discos;
    int movimentos;
    char data[20];
} HistRegistro;

// Nó da lista de histórico
typedef struct NoHist {
    HistRegistro reg;
    struct NoHist* prox;
} NoHist;

// Histórico
typedef struct {
    NoHist* inicio;
    char arquivo[50];
} Historico;

// Disco
Disco* criar_disco(int tam) {
    Disco* d = (Disco*)malloc(sizeof(Disco));
    if (!d) {
        perror("Erro ao alocar disco");
        exit(EXIT_FAILURE);
    }
    d->tam = tam;
    return d;
}

// Torre
void init_torre(Torre* t, char nome) {
    t->nome = nome;
    t->topo = -1;
}

int push_disco(Torre* t, Disco* d) {
    if (t->topo == 9) {
        printf("Torre cheia.\n");
        return 0;
    }
    if (t->topo != -1 && d->tam > t->pilha[t->topo]->tam) {
        printf("Movimento inválido.\n");
        return 0;
    }
    t->pilha[++t->topo] = d;
    return 1;
}

Disco* pop_disco(Torre* t) {
    if (t->topo == -1) {
        printf("Torre vazia.\n");
        return NULL;
    }
    return t->pilha[t->topo--];
}
// Exibe torres no terminal
void mostrar_torres(Torre torres[], int n_discos) {
    printf("\n");
    int largura_max = n_discos * 2 - 1;

    for (int i = n_discos - 1; i >= 0; i--) {
        for (int t = 0; t < 3; t++) {
            int espaco;
            if (torres[t].topo >= i) {
                int tam = torres[t].pilha[i]->tam;
                int largura = tam * 2 - 1;
                espaco = (largura_max - largura) / 2;
                for (int k = 0; k < espaco; k++) printf(" ");
                for (int k = 0; k < largura; k++) printf("■");
                for (int k = 0; k < espaco; k++) printf(" ");
            } else {
                espaco = (largura_max - 1) / 2;
                for (int k = 0; k < espaco; k++) printf(" ");
                printf("|");
                for (int k = 0; k < espaco; k++) printf(" ");
            }
            printf("     ");
        }
        printf("\n");
    }

    for (int t = 0; t < 3; t++) {
        for (int k = 0; k < largura_max; k++) printf("-");
        printf("     ");
    }
    printf("\n");

    for (int t = 0; t < 3; t++) {
        int espaco = (largura_max - strlen("TORRE X")) / 2;
        for (int k = 0; k < espaco; k++) printf(" ");
        printf("TORRE %c", torres[t].nome);
        for (int k = 0; k < espaco + (strlen("TORRE X") % 2 == 0 ? 0 : 1); k++) printf(" ");
        printf("     ");
    }
    printf("\n\n");
}

// Histórico
void init_historico(Historico* h) {
    h->inicio = NULL;
    strcpy(h->arquivo, "historico_hanoi.txt");
}

void carregar_historico(Historico* h) {
    FILE* f = fopen(h->arquivo, "r");
    if (!f) return;

    char linha[256];
    HistRegistro temp;
    while (fgets(linha, sizeof(linha), f)) {
        linha[strcspn(linha, "\n")] = 0;
        if (sscanf(linha, "%49[^;];%d;%d;%19s", temp.jogador, &temp.discos, &temp.movimentos, temp.data) == 4) {
            NoHist* novo = (NoHist*)malloc(sizeof(NoHist));
            if (!novo) {
                perror("Erro ao alocar histórico");
                exit(EXIT_FAILURE);
            }
            novo->reg = temp;
            novo->prox = h->inicio;
            h->inicio = novo;
        }
    }
    fclose(f);
}

void salvar_registro(Historico* h, const char* jogador, int discos, int movimentos) {
    NoHist* novo = (NoHist*)malloc(sizeof(NoHist));
    if (!novo) {
        perror("Erro ao alocar nó");
        exit(EXIT_FAILURE);
    }

    strcpy(novo->reg.jogador, jogador);
    novo->reg.discos = discos;
    novo->reg.movimentos = movimentos;

    time_t timer;
    struct tm* info;
    time(&timer);
    info = localtime(&timer);
    strftime(novo->reg.data, 20, "%Y-%m-%d %H:%M:%S", info);

    novo->prox = h->inicio;
    h->inicio = novo;

    FILE* f = fopen(h->arquivo, "a");
    if (!f) {
        perror("Erro ao salvar histórico");
        return;
    }
    fprintf(f, "%s;%d;%d;%s\n", jogador, discos, movimentos, novo->reg.data);
    fclose(f);
}

void mostrar_historico(Historico* h) {
    if (!h->inicio) {
        printf("Histórico vazio.\n");
        return;
    }
    NoHist* atual = h->inicio;
    int i = 1;
    while (atual) {
        printf("--- Registro %d ---\n", i++);
        printf("Jogador: %s\n", atual->reg.jogador);
        printf("Discos: %d\n", atual->reg.discos);
        printf("Movimentos: %d\n", atual->reg.movimentos);
        printf("Data: %s\n", atual->reg.data);
        printf("------------------\n");
        atual = atual->prox;
    }
}
void mover_disco(Torre torres[], int origem, int destino) {
    if (torres[origem].topo < 0) {
        printf("Origem vazia.\n");
        return;
    }

    if (torres[destino].topo >= 0 &&
        torres[origem].pilha[torres[origem].topo]->tam > torres[destino].pilha[torres[destino].topo]->tam) {
        printf("Movimento inválido: disco maior sobre menor.\n");
        return;
    }

    torres[destino].pilha[++torres[destino].topo] = torres[origem].pilha[torres[origem].topo--];
}

int venceu(Torre torres[], int n_discos) {
    return torres[2].topo == n_discos - 1;
}

int main() {
    int n_discos;
    char jogador[50];
    Historico historico;
    init_historico(&historico);
    carregar_historico(&historico);

    printf("=== Torre de Hanói ===\n");
    printf("Nome do jogador: ");
    fgets(jogador, sizeof(jogador), stdin);
    jogador[strcspn(jogador, "\n")] = 0;

    printf("Quantidade de discos (3 a 8): ");
    scanf("%d", &n_discos);
    while (n_discos < 3 || n_discos > 8) {
        printf("Número inválido. Tente novamente: ");
        scanf("%d", &n_discos);
    }

    Torre torres[3];
    char nomes[] = {'A', 'B', 'C'};
    for (int i = 0; i < 3; i++) {
        torres[i].nome = nomes[i];
        torres[i].topo = -1;
    }

    for (int i = n_discos; i > 0; i--) {
        Disco* d = (Disco*)malloc(sizeof(Disco));
        d->tam = i;
        torres[0].pilha[++torres[0].topo] = d;
    }

    int origem, destino, movimentos = 0;
    while (!venceu(torres, n_discos)) {
        mostrar_torres(torres, n_discos);
        printf("Movimento #%d\n", movimentos + 1);
        printf("Origem (0=A, 1=B, 2=C): ");
        scanf("%d", &origem);
        printf("Destino (0=A, 1=B, 2=C): ");
        scanf("%d", &destino);

        if (origem < 0 || origem > 2 || destino < 0 || destino > 2) {
            printf("Entrada inválida.\n");
            continue;
        }

        mover_disco(torres, origem, destino);
        movimentos++;
    }

    mostrar_torres(torres, n_discos);
    printf("Parabéns, %s! Você venceu em %d movimentos.\n", jogador, movimentos);
    salvar_registro(&historico, jogador, n_discos, movimentos);

    printf("\nDeseja ver o histórico? (s/n): ");
    char opcao;
    scanf(" %c", &opcao);
    if (opcao == 's' || opcao == 'S') {
        mostrar_historico(&historico);
    }

    // Liberar memória
    for (int i = 0; i < 3; i++) {
        while (torres[i].topo >= 0) {
            free(torres[i].pilha[torres[i].topo--]);
        }
    }

    NoHist* atual = historico.inicio;
    while (atual) {
        NoHist* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    return 0;
}
