#include<stdio.h>
#include<stdlib.h>
#include<locale.h>
#include"corpo_teste.c"
#define ARQUIVO_HISTORIA "historia.txt"

int main() {
	setlocale(LC_ALL, "PORTUGUESE");
    Grafo g;
    g.total_nos = 0;
    int opcao;

    do {
        Menu();

        do {
            if (scanf("%d", &opcao) != 1) {
                printf(" Entrada invalida! Digite apenas numeros.\n");
                limpaBuffer();
                opcao = -1; 
            }
            else if(opcao < 1 || opcao > 4){
            	printf("Entrada inválida! Digite um número entre 1 e 4.\n");
            	limpaBuffer();
                opcao = -1; 
			}
        } while (opcao < 1 || opcao > 4);

        switch (opcao) {
            case 1:{
               	Jogador jogador;
            	if (!carregar_jogo(&jogador)){
               	 	printf("\nNenhum jogo guardado encontrado.\n");
                	printf("Inicia um Novo Jogo primeiro.\n");
                	continue;
            	}
            	printf("\nA retomar o jogo...\n");
            	printf("No: %d | Vida: %d | Sanidade: %d | Ciclos: %d\n",jogador.no_atual, jogador.vida, jogador.sanidade, jogador.num_ciclos);
				int resultado= jogar(&g, &jogador);
				if (resultado == 0) {
                    char nome[50];
                    printf("Digite o seu nome, bravo jogador, para ser imortalizado: ");
                    scanf("%49s", nome);
                    guardar_jogador(nome, jogador.num_ciclos);
                    recorde(nome, jogador.num_ciclos);
                    apagar_save();
                }
				break;
			}
            case 2: {
            	system("cls");
                if (!carregar_historia(ARQUIVO_HISTORIA, &g)) {
                    printf("\n[ERRO] Nao foi possivel carregar '%s'.\n", ARQUIVO_HISTORIA);
                    break;
                }
                Jogador j;
                j.vida = 100;
                j.sanidade = 100;
                j.arma_equipada.bonus_ataque = 0;
				strncpy(j.arma_equipada.nome, "Punhos", sizeof(j.arma_equipada.nome) - 1);
				j.arma_equipada.nome[sizeof(j.arma_equipada.nome) - 1] = '\0';
                j.num_armas = 0;
                j.num_pocoes = 0;
                j.no_atual = NO_INICIAL;
                j.ultimo_checkpoint = NO_INICIAL;
                j.num_ciclos = 0;
				int resultado = jogar(&g, &j);
                if (resultado == 0){
                    char nome[50];
                    printf("Digite o seu nome, bravo jogador, para ser imortalizado: ");
                    scanf("%49s", nome);
                    guardar_jogador(nome, j.num_ciclos);
                    recorde(nome, j.num_ciclos);
                    apagar_save();
                }
                break;
            }
            case 3:
                // TODO: Maior Pontuacao
                printf("\n===============================================================\n");
				printf("                     PONTUACAO\n");
				printf("===============================================================\n");
				listar_jogadores();
				printf("===============================================================\n");

                break;
            case 4:
            	system("cls");
                printf("\nAte a proxima, Oiner...\n");
                break;
        }
    } while (opcao != 4);

    return 0;
}
