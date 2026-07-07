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
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {} /* limpa o buffer */
                opcao = -1; 
            }
        } while (opcao < 1 || opcao > 4);

        switch (opcao) {
            case 1:{
               	Jogador jogador;
            	if (!carregar_jogo(&jogador)) {
               	 	printf("\nNenhum jogo guardado encontrado.\n");
                	printf("Inicia um Novo Jogo primeiro.\n");
                	continue;
            	}
            	printf("\nA retomar o jogo...\n");
            	printf("No: %d | Vida: %d | Sanidade: %d | Ciclos: %d\n",jogador.no_atual, jogador.vida, jogador.sanidade, jogador.num_ciclos);

            	jogar(&grafo, &jogador);
				apagar_save();
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
                j.no_atual = NO_INICIAL;
                j.ultimo_checkpoint = NO_INICIAL;
                j.num_ciclos = 0;
                jogar(&g, &j);
                break;
            }
            case 3:
                // TODO: Maior Pontuacao
                printf("\n[Maior Pontuacao - ainda por fazer]\n");
                break;
            case 4:
                printf("\nAte a proxima, Oiner...\n");
                break;
        }
    } while (opcao != 4);

    return 0;
}
