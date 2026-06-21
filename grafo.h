#define MAX_NOS 200
#define MAX_TEXTO 1024
#define MAX_OPCOES 6
#define MAX_LINHA 1024
 
#define NO_INICIAL 4   // ponto de respawn do ciclo 
 
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
    int    id;
    char   texto[MAX_TEXTO];
    int    num_opcoes;
    Aresta opcoes[MAX_OPCOES]; // a lista de adjacencia deste nó
    int    existe;       // este id foi carregado correctamente do ficheiro, confirma por precaução, pode ser 1 ou 0 ou booleano
    int    tem_inimigo;  // confirma se no nó tem um combate, vamos fazer um loop para as opçoes de combate, mesma coisa que o existe
    Inimigo inimigo;	// Dados do inimigo, valido so se teminimigo
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
} Jogador;
