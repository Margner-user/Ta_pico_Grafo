#include "grafo.h"

void Menu() {
    printf("\n");
    printf("===============================================================\n");
    printf("  ____  _   _ _____ __  __   ____   ___  ____  __  __ _____ \n");
    printf(" / __ \\| | | | ____|  \\/  | |  _ \\ / _ \\|  _ \\|  \\/  | ____|\n");
    printf("| |  | | | | |  _| | |\\/| | | | | | | | | |_) | |\\/| |  _|  \n");
    printf("| |__| | |_| | |___| |  | | | |_| | |_| |  _ <| |  | | |___ \n");
    printf(" \\___\\_\\\\___/|_____|_|  |_| |____/ \\___/|_| \\_\\_|  |_|_____|\n");
    printf("\n");
    printf("                 N A   F L O R E S T A\n");
    printf("===============================================================\n");
    printf("\n");
    printf("                 1 - Resumir Hist�ria\n");
    printf("                 2 - Novo Jogo\n");
    printf("                 3 - Maior Pontua��o\n");
    printf("                 4 - Sair\n");
    printf("\n");
    printf("===============================================================\n");
    printf("Escolha uma opcao: ");
}

int rolar_d20() {
    return (rand() % 20) + 1;
}
//  Aguarda ENTER (usado nos flashbacks)
static void esperar_enter(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    printf("[Prima ENTER para continuar...]\n");
    getchar();
}
static void remover_quebra_linha(char *linha) {
/*O fgets estava a me dar erro, 
pesquisei e parece que isso acontece quando não se remove o '\n' e '\r' do fim da linha */
    size_t len = strlen(linha);
    if (len > 0 && linha[len - 1] == '\n'){
      linha[len - 1] = '\0';
    }
    if (len > 1 && linha[len - 2] == '\r'){
      linha[len - 2] = '\0';
    }
}

//----------------Parser---------------------------
 //Btw ainda não existe mas garantir nó vai basicamente criar o nó
int carregar_historia(const char *caminho, Grafo *g) {
    FILE *f = fopen(caminho, "r");
    if (f == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", caminho);
        return 0;
    }
    g->total_nos = 0;
    char linha[MAX_LINHA];
    No  *no_atual = NULL;

    while (fgets(linha, sizeof(linha), f) != NULL) {
        remover_quebra_linha(linha);

        /* ignora linhas vazias e comentarios */
        if (linha[0] == '\0' || linha[0] == '#') continue;

        if (strncmp(linha, "ID:", 3) == 0) {
            no_atual = garantir_no(g, atoi(linha + 3));
        //Btw ainda não existe mas "garantir nó" vai basicamente criar o nó
        }
        else if (strncmp(linha, "TEXTO:", 6) == 0) {
            if (no_atual != NULL) {
                strncpy(no_atual->texto, linha + 6, MAX_TEXTO - 1);
                no_atual->texto[MAX_TEXTO - 1] = '\0';
                /* flashback detectado pelo prefixo [MEMORIA */
                no_atual->e_flashback = (strncmp(no_atual->texto, "[MEMORIA", 8) == 0) ? 1 : 0;
            }
        }
        else if (strncmp(linha, "CHECKPOINT:", 11) == 0) {
            if (no_atual != NULL)
                no_atual->e_checkpoint = atoi(linha + 11);
        }
        else if (strncmp(linha, "INIMIGO:", 8) == 0) {
            if (no_atual != NULL) {
                char campo[MAX_LINHA];
                strncpy(campo, linha + 8, sizeof(campo) - 1);
                campo[sizeof(campo) - 1] = '\0';

                char *nome_str = strtok(campo, "|");

                char *vida_str = strtok(NULL,"|");
                char *ataque_str = strtok(NULL, "|");
                char *defesa_str = strtok(NULL, "|");
                if (nome_str && vida_str && ataque_str && defesa_str) {
                    strncpy(no_atual->inimigo.nome, nome_str, sizeof(no_atual->inimigo.nome) - 1);
                    no_atual->inimigo.nome[sizeof(no_atual->inimigo.nome) - 1] = '\0';
                    no_atual->inimigo.vida = atoi(vida_str);
                    no_atual->inimigo.bonus_ataque = atoi(ataque_str);
                    no_atual->inimigo.defesa = atoi(defesa_str);
                    no_atual->tem_inimigo = 1;
                }
            }
        }
        else if (strncmp(linha, "OPCAO:", 6) == 0) {
            if (no_atual != NULL && no_atual->num_opcoes < MAX_OPCOES) {
                char campo[MAX_LINHA];
                strncpy(campo, linha + 6, sizeof(campo) - 1);
                campo[sizeof(campo) - 1] = '\0';
                //Basicamente o tokenazer do java, vou separar o campo da opção porque tem peso, destino e etc...
                char *numero_str= strtok(campo, "|");
                char *texto_str = strtok(NULL,  "|");
                char *peso_str = strtok(NULL,  "|");
                char *destino_str = strtok(NULL,  "|");

                if (numero_str && texto_str && peso_str && destino_str) {
                    Aresta *a = &no_atual->opcoes[no_atual->num_opcoes];
                    a->numero = atoi(numero_str);
                    strncpy(a->texto, texto_str, sizeof(a->texto)-1);
                    a->texto[sizeof(a->texto) - 1] = '\0';
                    a->peso = atoi(peso_str);
                    a->destino = atoi(destino_str);
                    no_atual->num_opcoes++;
                }
            }
        }
        else if (strcmp(linha, "FIM") == 0) {
            no_atual = NULL;
        }
    }

    fclose(f);
    return 1;
}//Fim do parser
// Tenta um ataque: rola d20 + bonus de ataque vs. defesa do inimigo,devolve 1 se acertou (escreve dano em *dano_resultante), 0 se falhou.

int rolar_ataque(int bonus_ataque, int defesa_alvo, int *dano_resultante) {
    int dado = rolar_d20();
    int total =dado + bonus_ataque;

    printf("  [d20: %d + bonus %d = %d vs defesa %d]\n", dado, bonus_ataque, total, defesa_alvo);

    if (total >= defesa_alvo) {
        int dano = total-defesa_alvo;
        if (dano < 1){
          dano = 1;
        }
        *dano_resultante = dano;
        return 1;
    }
    *dano_resultante = 0;
    return 0;
}

int combate(Jogador *j, Inimigo *inimigo){
    const int JOGADOR_BONUS_ATAQUE = 4;
    const int JOGADOR_DEFESA = 12;
    int vida_inimigo = inimigo->vida;

    printf("\n*** COMBATE: %s aparece! ***\n", inimigo->nome);
    printf("(%s -> Vida: %d | Ataque: +%d | Defesa: %d)\n", inimigo->nome, vida_inimigo,
           inimigo->bonus_ataque, inimigo->defesa);

    int turno = 1;
    while (j->vida > 0 && vida_inimigo > 0) {
        printf("\n--- Turno %d ---\n", turno);
        /* turno do jogador */
        int dano;
        if (rolar_ataque(JOGADOR_BONUS_ATAQUE, inimigo->defesa, &dano)) {
            vida_inimigo -= dano;
            if (vida_inimigo < 0){
              vida_inimigo = 0;
            }
            printf("Oiner acerta! Causa %d de dano. (%s: %d vida)\n",
                   dano, inimigo->nome, vida_inimigo);
        } else {
            printf("Oiner ataca, mas falha o golpe.\n");
        }
        if (vida_inimigo <= 0) break;
        /* turno do inimigo */
        if (rolar_ataque(inimigo->bonus_ataque, JOGADOR_DEFESA, &dano)) {
            j->vida -= dano;
            if (j->vida < 0) j->vida = 0;
            printf("%s acerta Oiner! Causa %d de dano. (Oiner: %d vida)\n",
                   inimigo->nome, dano, j->vida);
        } else {
            printf("%s ataca, mas erra o golpe.\n", inimigo->nome);
        }

        turno++;
    }
    if (j->vida <= 0) {
        printf("\nOiner cai, derrotado por %s...\n", inimigo->nome);
        return 0;
    }
    printf("\nOiner venceu o combate contra %s!\n", inimigo->nome);
    return 1;
}

void mostrar_status(Jogador *j) {
    printf("\n----------------------------------------\n");
    printf(" Vida: %d   |   Sanidade: %d", j->vida, j->sanidade);
    if (j->num_ciclos > 0)
        printf("   |   Ciclos: %d", j->num_ciclos);
    printf("\n----------------------------------------\n");
}
