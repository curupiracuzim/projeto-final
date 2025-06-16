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

// Registro de Jogo
typedef struct {
    char jogador[50];
    int discos;
    int movimentos;
    char data[20];
    char status[20];
} RegistroJogo;

// Nó do Histórico
typedef struct NoHistorico {
    RegistroJogo registro_jogo;
    struct NoHistorico* prox;
} NoHistorico;

// Gerenciador de Histórico
typedef struct {
    NoHistorico* inicio;
    char arquivo[50];
} GerenciadorHistorico;

Disco* criar_disco(int tam) {
    Disco* d = (Disco*)malloc(sizeof(Disco));
    if (!d) {
        perror("Erro ao alocar disco");
        exit(EXIT_FAILURE);
    }
    d->tam = tam;
    return d;
}

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

void init_gerenciador_historico(GerenciadorHistorico* gh) {
    gh->inicio = NULL;
    strcpy(gh->arquivo, "historico_hanoi.txt");
}

void carregar_historico(GerenciadorHistorico* gh) {
    FILE* f = fopen(gh->arquivo, "r");
    if (!f) return;

    char linha[256];
    RegistroJogo temp;
    while (fgets(linha, sizeof(linha), f)) {
        linha[strcspn(linha, "\n")] = 0;
        if (sscanf(linha, "%49[^;];%d;%d;%19[^;];%19s", temp.jogador, &temp.discos, &temp.movimentos, temp.data, temp.status) == 5) {
            NoHistorico* novo = (NoHistorico*)malloc(sizeof(NoHistorico));
            if (!novo) {
                perror("Erro ao alocar histórico");
                exit(EXIT_FAILURE);
            }
            novo->registro_jogo = temp;
            novo->prox = gh->inicio;
            gh->inicio = novo;
        }
    }
    fclose(f);
}

void salvar_registro(GerenciadorHistorico* gh, const char* jogador, int discos, int movimentos, const char* status_jogo) {
    NoHistorico* novo = (NoHistorico*)malloc(sizeof(NoHistorico));
    if (!novo) {
        perror("Erro ao alocar nó");
        exit(EXIT_FAILURE);
    }

    strcpy(novo->registro_jogo.jogador, jogador);
    novo->registro_jogo.discos = discos;
    novo->registro_jogo.movimentos = movimentos;
    strcpy(novo->registro_jogo.status, status_jogo);

    time_t tempo_atual;
    struct tm* info_tempo;
    time(&tempo_atual);
    info_tempo = localtime(&tempo_atual);
    strftime(novo->registro_jogo.data, 20, "%Y-%m-%d %H:%M:%S", info_tempo);

    novo->prox = gh->inicio;
    gh->inicio = novo;

    FILE* arquivo_historico = fopen(gh->arquivo, "a");
    if (!arquivo_historico) {
        perror("Erro ao salvar histórico");
        return;
    }
    fprintf(arquivo_historico, "%s;%d;%d;%s;%s\n", jogador, discos, movimentos, novo->registro_jogo.data, novo->registro_jogo.status);
    fclose(arquivo_historico);
}

void mostrar_historico(GerenciadorHistorico* gh) {
    if (!gh->inicio) {
        printf("Histórico vazio.\n");
        return;
    }
    // Exibir do mais antigo para o mais recente
    int count = 0;
    NoHistorico* atual = gh->inicio;
    while (atual) {
        count++;
        atual = atual->prox;
    }
    NoHistorico** vetor = (NoHistorico**)malloc(count * sizeof(NoHistorico*));
    atual = gh->inicio;
    for (int i = count - 1; i >= 0; i--) {
        vetor[i] = atual;
        atual = atual->prox;
    }
    for (int i = 0; i < count; i++) {
        printf("--- Registro %d ---\n", i + 1);
        printf("Jogador: %s\n", vetor[i]->registro_jogo.jogador);
        printf("Discos: %d\n", vetor[i]->registro_jogo.discos);
        printf("Movimentos: %d\n", vetor[i]->registro_jogo.movimentos);
        printf("Data: %s\n", vetor[i]->registro_jogo.data);
        printf("Status: %s\n", vetor[i]->registro_jogo.status);
        printf("------------------\n");
    }
    free(vetor);
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

// Função para limpar o buffer do teclado
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int numero_discos;
    char nome_jogador[50];
    GerenciadorHistorico gerenciador_historico;
    init_gerenciador_historico(&gerenciador_historico);
    carregar_historico(&gerenciador_historico);

    printf("=== Torre de Hanói ===\n");
    printf("Nome do jogador: ");
    if (!fgets(nome_jogador, sizeof(nome_jogador), stdin)) {
        printf("Erro ao ler nome.\n");
        return 1;
    }
    nome_jogador[strcspn(nome_jogador, "\n")] = 0;

    printf("Quantidade de discos (3 a 8): ");
    while (scanf("%d", &numero_discos) != 1 || numero_discos < 3 || numero_discos > 8) {
        printf("Número inválido. Tente novamente: ");
        limpar_buffer();
    }
    limpar_buffer();

    Torre torres_jogo[3];
    char nomes_torres[] = {'A', 'B', 'C'};
    for (int i = 0; i < 3; i++) {
        torres_jogo[i].nome = nomes_torres[i];
        torres_jogo[i].topo = -1;
    }

    for (int i = numero_discos; i > 0; i--) {
        Disco* novo_disco = (Disco*)malloc(sizeof(Disco));
        novo_disco->tam = i;
        torres_jogo[0].pilha[++torres_jogo[0].topo] = novo_disco;
    }

    int torre_origem, torre_destino, total_movimentos = 0;
    int jogo_ativo = 1;

    while (jogo_ativo && !venceu(torres_jogo, numero_discos)) {
        mostrar_torres(torres_jogo, numero_discos);
        printf("Movimento #%d\n", total_movimentos + 1);
        printf("Origem (0=A, 1=B, 2=C, -1 para sair): ");
        if (scanf("%d", &torre_origem) != 1) {
            printf("Entrada inválida.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        if (torre_origem == -1) {
            jogo_ativo = 0;
            break;
        }

        printf("Destino (0=A, 1=B, 2=C): ");
        if (scanf("%d", &torre_destino) != 1) {
            printf("Entrada inválida.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        if (torre_origem < 0 || torre_origem > 2 || torre_destino < 0 || torre_destino > 2) {
            printf("Entrada inválida. Por favor, insira um número entre 0 e 2 para origem e destino.\n");
            continue;
        }

        Disco* disco_movido = pop_disco(&torres_jogo[torre_origem]);
        if (disco_movido == NULL) {
            printf("Torre de origem vazia. Escolha outra torre.\n");
            continue;
        }
        if (!push_disco(&torres_jogo[torre_destino], disco_movido)) {
            push_disco(&torres_jogo[torre_origem], disco_movido);
            continue;
        }
        total_movimentos++;
    }

    if (venceu(torres_jogo, numero_discos)) {
        mostrar_torres(torres_jogo, numero_discos);
        printf("Parabéns, %s! Você venceu em %d movimentos.\n", nome_jogador, total_movimentos);
        salvar_registro(&gerenciador_historico, nome_jogador, numero_discos, total_movimentos, "Venceu");
    } else {
        printf("\nVocê abandonou o jogo.\n");
        salvar_registro(&gerenciador_historico, nome_jogador, numero_discos, total_movimentos, "Abandonou");
    }

    printf("\nDeseja ver o histórico? (s/n): ");
    char opcao_historico;
    if (scanf(" %c", &opcao_historico) == 1 && (opcao_historico == 's' || opcao_historico == 'S')) {
        limpar_buffer();
        mostrar_historico(&gerenciador_historico);
    } else {
        limpar_buffer();
    }

    for (int i = 0; i < 3; i++) {
        while (torres_jogo[i].topo >= 0) {
            free(torres_jogo[i].pilha[torres_jogo[i].topo--]);
        }
    }

    NoHistorico* atual_historico = gerenciador_historico.inicio;
    while (atual_historico) {
        NoHistorico* temp_historico = atual_historico;
        atual_historico = atual_historico->prox;
        free(temp_historico);
    }

    return 0;
}