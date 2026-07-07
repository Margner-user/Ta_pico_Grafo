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
    printf("                 1 - Resumir História\n");
    printf("                 2 - Novo Jogo\n");
    printf("                 3 - Maior Pontuação\n");
    printf("                 4 - Sair\n");
    printf("\n");
    printf("===============================================================\n");
    printf("Escolha uma opcao: ");
}

No *obter_no(Grafo *g, int id) {
	int i;
    for (i = 0; i < g->total_nos; i++) {
        if (g->nos[i].id == id) {
            return &g->nos[i];
        }
    }
    return NULL;
}

/* Garante que existe uma entrada para este id (cria se necessario) */
No *garantir_no(Grafo *g, int id) {
    No *no = obter_no(g, id);
    if (no != NULL)
        return no;
    no= &g->nos[g->total_nos];
    no->id= id;
    no->texto[0] = '\0';
    no->num_opcoes = 0;
    no->existe = 1;
    no->tem_inimigo = 0;
    no->e_checkpoint= 0;
    no->e_flashback = 0;
    g->total_nos++;
    return no;
}//Novidade(03 de Julho)
void ordenar_opcoes_desc(Aresta *arr, int n) {
	int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j].peso < arr[j + 1].peso) {
                Aresta tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}//Novidade(03 de Julho)
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
pesquisei e parece que isso acontece quando nÃƒÂ£o se remove o '\n' e '\r' do fim da linha */
    size_t len = strlen(linha);
    if (len > 0 && linha[len - 1] == '\n'){
      linha[len - 1] = '\0';
    }
    if (len > 1 && linha[len - 2] == '\r'){
      linha[len - 2] = '\0';
    }
}

//----------------Parser---------------------------
 //Btw  garantir_no ainda nao existe mas vai basicamente criar o nÃƒÂ³
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
        //Btw ainda nÃƒÂ£o existe mas "garantir nÃƒÂ³" vai basicamente criar o nÃƒÂ³
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
                //Basicamente o tokenazer do java, vou separar o campo da opÃƒÂ§ÃƒÂ£o porque tem peso, destino e etc...
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

int resolver_rolagem(No *no, Aresta *escolhida, int *penalizacao) {
    *penalizacao = 0;
    int rolagem = rolar_d20();
    printf("\n[Rolagem de dado: tiraste %d, dificuldade era %d]\n", rolagem, escolhida->peso);
    if (rolagem >= escolhida->peso) {
        printf(">> Sucesso! Segues o caminho que escolheste.\n");
        return escolhida->destino;
    }
    Aresta validas[MAX_OPCOES];
    int n_validas = 0;
    int i;
    for (i = 0; i < no->num_opcoes; i++)
        validas[n_validas++] = no->opcoes[i];
    ordenar_opcoes_desc(validas, n_validas);
    for (i = 0; i < n_validas; i++) {
        if (rolagem >= validas[i].peso) {
            printf(">> Falhaste por %d pontos. O destino desvia-te para: \"%s\"\n",
                   escolhida->peso - rolagem, validas[i].texto);
            return validas[i].destino;
        }
    }
    Aresta *pior = &validas[n_validas - 1];
    *penalizacao = pior->peso - rolagem;
    if (*penalizacao < 0) 
        *penalizacao = 0; 
    printf(">> Rolagem demasiado baixa! Nem o caminho mais facil estava ao teu alcance.\n");
    if (pior->peso == 0)
        printf(">> O confronto e inevitavel!\n");
    else
        printf(">> Oiner e forcado para: \"%s\" (-%d sanidade)\n", pior->texto, *penalizacao);
    return pior->destino;
}//Novidade(03 de Julho)

void reiniciar_ciclo(Jogador *j){
    j->vida = 100;
    j->sanidade = 100;
    j->no_atual = j->ultimo_checkpoint;
    j->num_ciclos++;
}

void jogar(Grafo *g, Jogador *j) {
	srand((unsigned int)time(NULL));   // seed da aleatoriedade, uma vez por partida
    while (1) {
        No *no = obter_no(g, j->no_atual);
        if (no == NULL) {
            printf("\n[ERRO] No %d nao encontrado. A terminar.\n", j->no_atual);
            return;
        }
        if (no->e_checkpoint)
            j->ultimo_checkpoint = no->id;
        //Mostra o texto do no 
        printf("\n========================================\n");
        if (no->e_flashback) {
            printf("       * * * FRAGMENTO DE MEMORIA * * *\n");
            printf("========================================\n");
            printf("%s\n", no->texto);
            printf("========================================\n");
            mostrar_status(j);
            esperar_enter();
        } else {
            printf("%s\n", no->texto);
            printf("========================================\n");
            mostrar_status(j);
        }
        if (j->vida <= 0 || j->sanidade <= 0) {
            if (j->sanidade <= 0)
                printf("\nA mente de Oiner colapsa sob o peso da insanidade...\n");
            else
                printf("\nOiner sucumbe aos ferimentos...\n");
            reiniciar_ciclo(j);
            continue;
        }
        // fim da historia 
        if (no->num_opcoes == 0) {
            printf("\n[Fim da historia. Obrigado por jogar.]\n");
            return;
        }
     	int i;
        for (i = 0; i < no->num_opcoes; i++) {
            Aresta *a = &no->opcoes[i];
            if (no->tem_inimigo && a->peso == 0)
                printf("%d - %s [COMBATE]\n", a->numero, a->texto);
            else
                printf("%d - %s (dificuldade %d)\n", a->numero, a->texto, a->peso);
        }
        //le escolha do jogador 
        int escolha = -1;
        printf("\nEscolhe uma opcao: ");
        int res = scanf("%d", &escolha);// Vai retornar 1 se for bem lido 0 se for mau lido, crt+z para sair do loop/jogo
        if (res == EOF) {
            printf("\n[EOF. A sair.]\n");
            return;
        }
        if (res != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Entrada invalida.\n");
            continue;
        }
        Aresta *escolhida = NULL;
        
        for (i = 0; i < no->num_opcoes; i++) {
            if (no->opcoes[i].numero == escolha) {
                escolhida = &no->opcoes[i];
                break;
            }
        }
        if (escolhida == NULL) {
            printf("Opcao invalida, tenta novamente.\n");
            continue;
        }

        // se o jogador escolheu atacar directamente (peso 0) 
        if (no->tem_inimigo && escolhida->peso == 0) {
            int venceu = combate(j, &no->inimigo);
            if (!venceu) continue;
            j->no_atual = escolhida->destino;
            continue;
        }
        int penalizacao = 0;
        int destino = resolver_rolagem(no, escolhida, &penalizacao);
        if (penalizacao > 0) {
            j->sanidade -= penalizacao;
            if (j->sanidade < 0) j->sanidade = 0;
        }
        //verifica se o destino e uma opcao de combate (peso 0) 
        int e_combate_forcado = 0;
        
        if (no->tem_inimigo) {
            for (i = 0; i < no->num_opcoes; i++) {
                if (no->opcoes[i].peso == 0 &&
                    no->opcoes[i].destino == destino) {
                    e_combate_forcado = 1;
                    break;
                }
            }
        }
        if (e_combate_forcado) {
            int venceu = combate(j, &no->inimigo);
            if (!venceu) continue;
        }
        j->no_atual = destino;
    }
}////Novidade(03 de Julho)

// Salvar Progresso
//
#define FICHEIRO_SAVE "save.bin"
#define SAVE_SIGNATURE "SAVE"   
int guardar_jogo(const Jogador *j) {
    FILE *f = fopen(FICHEIRO_SAVE, "wb");
    if (!f) return 0;
    fwrite(SAVE_SIGNATURE, sizeof(char), strlen(SAVE_SIGNATURE), f); // escreve "SAVE", para localizar o ficheiro correcto
    fwrite(j, sizeof(Jogador), 1, f);                                // escreve jogador
    fclose(f);
    printf("Jogo guardado!\n");
    return 1;
}
int carregar_jogo(Jogador *j) {
    FILE *f = fopen(FICHEIRO_SAVE, "rb");
    if (!f) return 0;
    char assinatura[5] = {0}; // espaço para "SAVE" + '\0'
    fread(assinatura, sizeof(char), strlen(SAVE_SIGNATURE), f);
    if (strcmp(assinatura, SAVE_SIGNATURE) != 0) {
        printf("Save inválido ou corrompido.\n");
        fclose(f);
        return 0;
    }
    fread(j, sizeof(Jogador), 1, f);
    fclose(f);
    printf("Jogo carregado!\n");
    return 1;
}
void apagar_save(void) {
    remove(FICHEIRO_SAVE);
    printf("Save apagado.\n");
}


