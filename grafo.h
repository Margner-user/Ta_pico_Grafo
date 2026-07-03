#define MAX_NOS 200
#define MAX_TEXTO 1024
#define MAX_OPCOES 6
#define MAX_LINHA 1024
 
#define NO_INICIAL 4   // ponto de respawn do ciclo 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
// Dados do inimigo
typedef struct {
    char nome[64];
    int  vida;
    int  bonus_ataque;// somado ao d20 no ataque do inimigo, vai fazer parte de um sistema por definir
    int  defesa; // valor que o jogador precisa de igualar/superar para acertar, mesma coisa que o bonus de ataque
} Inimigo;
 
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
    Inimigo inimigo;	// Dados do inimigo, valido so se teminimigo
    int e_checkpoint;  //  grava checkpoint ao entrar 
    int e_flashback;   //cena de memoria 
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
} Jogador;

//Menu
void Menu();

//Carregar ficheiro e grafo
void jogar(Grafo *g, Jogador *j);//Função principal
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
