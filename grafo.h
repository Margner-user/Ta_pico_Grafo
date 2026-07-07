#define MAX_NOS 200
#define MAX_TEXTO 1024
#define MAX_OPCOES 6
#define MAX_LINHA 1024
 
#define NO_INICIAL 4   // ponto de respawn do ciclo 
#define MAX_POCOES 20         // quantas pocoes cabem na mochila
#define MAX_ARMAS_MOCHILA 10  // quantas armas (nao equipadas) cabem na mochila
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//Update recordes
#define RECORDE_H
#define FICHEIRO_RECORDE "recordes.bin"
#define RECORDE_INFINITO 999999
//Struct tabela de recordes
typedef struct {
    char nome[50];
    int ciclos;
} JogadorRecorde;
// Tipos de item que um no pode conceder ao ser alcancado
typedef enum {
    ITEM_ARMA = 0,
    ITEM_POCAO_VIDA,
    ITEM_POCAO_SANIDADE
} TipoItem;

// Arma equipada pelo jogador (nome + bonus de ataque)
typedef struct {
    char nome[64];
    int  bonus_ataque;
} Arma;
// Dados do inimigo
typedef struct {
    char nome[64];
    int  vida;
    int  bonus_ataque;// somado ao d20 no ataque do inimigo, vai fazer parte de um sistema por definir
    int  defesa; // valor que o jogador precisa de igualar/superar para acertar, mesma coisa que o bonus de ataque
} Inimigo;

// Uma pocao guardada na mochila (cada uma mantem o seu proprio valor de cura,
// porque no historia.txt pocoes diferentes podem curar quantidades diferentes)
typedef struct {
    char nome[64];
    TipoItem tipo;   // ITEM_POCAO_VIDA ou ITEM_POCAO_SANIDADE
    int valor;       // quanto cura esta pocao especifica
} ItemPocao;
 
typedef struct {
    int  numero;        // número que vai aparecer na opção
    char texto[256];    // texto mostrado para a opcao 
    int  peso;           // dificuldade/peso 
    int  destino;       // ID do No para onde esta aresta aponta, basicamente o id
} Aresta;
 
typedef struct {
    int id;
    char texto[MAX_TEXTO];
    int num_opcoes;
    Aresta opcoes[MAX_OPCOES]; // a lista de adjacencia deste nó
    int existe;       // este id foi carregado correctamente do ficheiro, confirma por precaução, pode ser 1 ou 0 ou booleano
    int tem_inimigo;  // confirma se no nó tem um combate, vamos fazer um loop para as opçoes de combate, mesma coisa que o existe
    Inimigo inimigo;	// Dados do inimigo, valido so se tem inimigo
    int e_checkpoint;  //  grava checkpoint ao entrar 
    int e_flashback;   //cena de memoria 
    int da_item; // 1 se o no d  smth at all
    TipoItem tipo_item;
    char item_nome[64];
    int item_valor;
    int coletado; // 1 depois do jogador ja ter apanhado o item deste no (nao repete, mesmo entre ciclos)
} No;
 
typedef struct {
    No nos[MAX_NOS];
    int total_nos;
} Grafo;
 
// Estado do jogador 
 
typedef struct {
    int vida;
    int sanidade; //Pontos de sanidade muda em falhas de dado fora de combate
    int no_atual;
    int ultimo_checkpoint;  //id do ultimo no com CHECKPOINT:1 visitado 
    int num_ciclos;        // quantas vezes o ciclo foi reiniciado 
    Arma arma_equipada;    // arma atual do jogador (nome + bonus de ataque)
    Arma mochila_armas[MAX_ARMAS_MOCHILA]; // armas apanhadas mas nao equipadas
    int num_armas;                          // quantas armas estao na mochila
    ItemPocao pocoes[MAX_POCOES];           // pocoes na mochila (vida ou sanidade)
    int num_pocoes;                         // quantas pocoes estao na mochila
} Jogador;

//Menu
void Menu();

//Carregar ficheiro e grafo
int  jogar(Grafo *g, Jogador *j); // 0=fim normal, 1=guardou e saiu, 2=erro/EOF função principal
void reiniciar_ciclo(Jogador *j);
No  *obter_no(Grafo *g, int id);// Vai buscar o nó dentro do nosso grafo
No  *garantir_no(Grafo *g, int id);//Garante que o nó existe se não existir vai criar
int  carregar_historia(const char *caminho, Grafo *g);// Função de parsing, vai decodificar o que esta no ficheiro da historia para nós
//
void ordenar_opcoes_desc(Aresta *arr, int n);
int rolar_d20(void);//Função do rolo do dado
int resolver_rolagem(No *no, Aresta *escolhida, int *penalizacao);// VAi ajudar na movimentação por causa dos pesos
//
int rolar_ataque(int bonus_ataque, int defesa_alvo, int *dano_resultante);//Gera o dano que o jogador dá ou recebe,
int combate(Jogador *j, Inimigo *inimigo);//Faz o loop até que o inimigo ou o jogador estiver morto
void mostrar_status(Jogador *j);//Mostra a vida do jogador
//
int  guardar_jogo(const Jogador *j);//Para não se perder o progresso quando se sai do jogo
int  carregar_jogo(Jogador *j);
void apagar_save();
//complementares
void sleep_ms(int ms);
void limpaBuffer();

// Recordes
void guardar_jogador(char *nome, int ciclos);
void listar_jogadores();
void recorde(char *nome, int ciclos);
// Inventario (mochila) - so acessivel fora de combate
void adicionar_pocao(Jogador *j, const char *nome, TipoItem tipo, int valor);// guarda uma pocao apanhada na mochila
int  usar_pocao(Jogador *j, int indice);// bebe a pocao no indice (0-based), aplica o efeito e remove da mochila
void adicionar_arma(Jogador *j, Arma arma);// guarda uma arma apanhada na mochila (sem equipar automaticamente)
void equipar_arma(Jogador *j, int indice);// troca a arma equipada pela da mochila no indice (a antiga volta para a mochila)
void desequipar_arma(Jogador *j);// guarda a arma equipada na mochila e volta a lutar com os punhos
void abrir_inventario(Jogador *j);// menu da mochila (equipar/desequipar arma, beber pocao)
