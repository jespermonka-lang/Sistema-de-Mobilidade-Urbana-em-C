#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BAIRROS 105
#define INF 999999

// definiçao da struct que representa o grafo da cidade
typedef struct {
    int matriz[MAX_BAIRROS][MAX_BAIRROS]; // Matriz d'adjacencia para guardar as distantes (pesos das arestas)
    int num_bairros;                      // Variavel para guardar o numero total de bairros no grafo
    int num_ruas;                         // Variavel pra guardar o numero total de ruas/arestas
} Cidade;

// FUNÇAO DE INICIALISAÇAO E OS ALGORITMOS

// Initializa a struct da cidade com valores default
void inicializar_cidade(Cidade *c) {
    c->num_bairros = 0; // Initialize o numero de bairros a 0
    c->num_ruas = 0;    // Initialize o numero de ruas a 0
    for (int i = 0; i < MAX_BAIRROS; i++) { // Percorre as linhas da matriz
        for (int j = 0; j < MAX_BAIRROS; j++) { // Percorre as colunas da matriz
            if (i == j) c->matriz[i][j] = 0; // A distancia de um bairro até ele mesmo é sempre 0.
            else c->matriz[i][j] = INF;      // Por default, initializa a distancia a infinito(nao tem ruas)
        }
    }
}

// carrega o grafo a partir de um arquivo .txt ou .in
int carregar_arquivo_grafo(Cidade *c, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r"); // Abre o arquivo em modo leitura("r")
    if (arquivo == NULL) { // verifica se o arquivo existe ou s'il pode ter sido aberto.
        printf("Erro: Nao foi possivel abrir o arquivo '%s'. Verifique o nome e tente novamente.\n", nome_arquivo);
        return 0; // return 0 para indicar a falha no carregamento
    }

    inicializar_cidade(c); // Reinitializa a memoria da cidade antes de carregar dados novos

    int d1, d2; // Variavel auxiliares para capturar as parametras opcional da primeira linha
    char linha1[100]; // Tabela de caracter para ler a primera linha do arquivo
    if (fgets(linha1, sizeof(linha1), arquivo) != NULL) { // lê a 1era linha textual do arquivo
        // Tenta ler 4 inteiros (formato standard do arquivo : nós, arestas, config , config)
        int n = sscanf(linha1, "%d %d %d %d", &c->num_bairros, &c->num_ruas, &d1, &d2);
        if (n == 1) { // Se um um unico numero compacto foi lido (ex: 5701 ao inves de 5 7 0 1).
            int valor = c->num_bairros; // Copia o valor lido
            d2 = valor % 10; valor /= 10; // Extrai do ultimo chiffre (paramètre 2).
            d1 = valor % 10; valor /= 10; // Extrai o antes-ultimos chiffre (paramètre 1).
            c->num_ruas = valor % 10;     // Extrai o numero de ruas
            c->num_bairros = valor / 10;  // O resto vira o numero de bairros
        }
    }
    
    char linha_rua[100]; // Tabela de carater pra ler cada linha representando uma rua
    for (int i = 0; i < c->num_ruas; i++) { // loop pra ler cada rua definida dentro do arquivo
        if (fgets(linha_rua, sizeof(linha_rua), arquivo) != NULL) { // Lê a linha da rua atual
            int u, v, peso; // Variaveis para guardar o bairro de origem, de destino e a distancia
            // Tenta primeiro ler ao formato com virgulas (ex: 1, 2, 4)
            if (sscanf(linha_rua, "%d, %d, %d", &u, &v, &peso) != 3) {
                // Se falhou, lê ao formato com espaços (ex: 1 2 4) 
                sscanf(linha_rua, "%d %d %d", &u, &v, &peso);
            }
            // Vérifica se as indices dos bairros sao validas ( dentro dos limites do grafo)
            if (u >= 1 && u <= c->num_bairros && v >= 1 && v <= c->num_bairros) {
                c->matriz[u][v] = peso; // Adicione o peso no sentindo u -> v.
                c->matriz[v][u] = peso; // Adicione o mesmo peso no sentindo v -> u (grafo nao-direicionado).
            }
        }
    }

    fclose(arquivo); // Fecha o arquivo para liberar os recursos do sistema
    printf("Sucesso: Grafo carregado com %d bairros e %d ruas a partir de '%s'.\n", c->num_bairros, c->num_ruas, nome_arquivo);
    return 1; // Retorne 1 para indicar que o grafo foi carregado com sucesso
}

// DFS
void dfs(Cidade *c, int u, int visitado[]) {
    visitado[u] = 1; // Marca o bairro atual 'u' como visitado
    for (int v = 1; v <= c->num_bairros; v++) { // Percorre todos os bairros possiveis da cidade
        // Se uma uma rua existe entre 'u' e 'v' e que 'v' nao foi visitado
        if (c->matriz[u][v] != INF && !visitado[v]) {
            dfs(c, v, visitado); // call recursivamente a funçao DFS para o bairro 'v'
        }
    }
}

// Vérifica em segundo plano se o grafo é totalmente conectado (sem print um texto)
int verificar_conectividade_silencioso(Cidade *c) {
    if (c->num_bairros == 0) return 0; // Se nenhum grafo foi carregado, retourne falso (0).
    int visitado[MAX_BAIRROS] = {0};   // Initializa a tabela dos bairros visitados a zero
    dfs(c, 1, visitado); // comece uma DFS a partir do primeiro bairro 
    for (int i = 1; i <= c->num_bairros; i++) { // Percorre o resultado de todos os bairros
        if (!visitado[i]) return 0; // Se um dos bairros nao foi atingindo, o grafo nao é conectado
    }
    return 1; // Se todos os bairros foram visitados, retorne verdadeiro (1).
}

// FUNCIONALIDADE 1 : Analise completo da conectividade e detecçao dos bairros isolados
void analisar_conectividade(Cidade *c, int bairro_inicial) {
    if (c->num_bairros == 0) { // Vérificaçao da segurança
        printf("Erro: Nenhum grafo foi carregado ainda. Use a opcao 0.\n");
        return;
    }
    if (bairro_inicial < 1 || bairro_inicial > c->num_bairros) { // Vérifica se a entrada utilisador é valido
        printf("Erro: Bairro inicial invalido.\n");
        return;
    }
    int visitado[MAX_BAIRROS] = {0}; // Tabela pra seguir os bairros visitados durante a analise
    printf("\n--- ANALISE DE CONECTIVIDADE ---\n");
    dfs(c, bairro_inicial, visitado); // Execute a DFS desde o bairro escolhido pelo user
    printf("A partir do bairro %d os bairros alcancaveis sao: ", bairro_inicial);
    int primeiro = 1; // Indicador para formatar corretamente as virgulas durante a print
    for (int i = 1; i <= c->num_bairros; i++) { // Percorre a lista dos bairros 
        if (visitado[i] && i != bairro_inicial) { // Se o bairro foi atingindo( e nao é o ponto de origem)
            if (!primeiro) printf(", "); // Adicione uma virgula se nao e o primeiro elemento printed
            printf("%d", i); // Imprime o numero do bairro accessivel
            primeiro = 0;    // Desative o indicador do 1er elemeento
        }
    }
    printf("\n");
    
    int totalmente_conectada = 1; // Variavel pra guardar o estado global de connectividade da cidade
    for (int i = 1; i <= c->num_bairros; i++) { // Percorre todos os bairros
        if (!visitado[i]) { totalmente_conectada = 0; break; } // Se um bairro é faltado, a cidade nao é conexo
    }
    
    if (totalmente_conectada) {
        printf("A cidade e toda conectada\n"); // Print que todo mundo pode ter acesso a todo mundo
        printf("Nao ha bairros isolados\n");
    } else {
        printf("A cidade nao e toda conectada\n");
        int componente[MAX_BAIRROS] = {0}, qtd_conjuntos = 0; // Variaveis para grupar as zonas isoladas
        for (int i = 1; i <= c->num_bairros; i++) { // Percorre cada bairro da cidade
            if (componente[i] == 0) { // Se esse bairro ainda nao pertence a nenhum grupo identificado
                qtd_conjuntos++; // Incremente o contador de composantes connexos
                int v_comp[MAX_BAIRROS] = {0}; // Cria uma tabela temporaria de visita pra esse composante connexo
                dfs(c, i, v_comp); // descobre todos os bairros conectados a esse bairro especifico
                printf("Conjunto %d: ", qtd_conjuntos); // print o numero do grupo isolado
                int p_comp = 1; // Indicador de formatagem de orint pra esse grupo
                for (int j = 1; j <= c->num_bairros; j++) { // Percorre de novo a lista compelta
                    if (v_comp[j]) { // Se o baiiro 'j' faz parte do composante
                        componente[j] = qtd_conjuntos; // Associa définitivamente o bairro ao grupo
                        if (!p_comp) printf(", "); // Formatagem das virgulas.
                        printf("%d", j); // print o bairro pertencendo ao grupo
                        p_comp = 0;
                    }
                }
                printf("\n");
            }
        }
    }
}

// Funçao recursiva auxiliar para print o caminho generado pelo algorimo de Dijkstra
void imprimir_caminho(int predecessor[], int j) {
    if (predecessor[j] == -1) { printf("%d", j); return; } // Caso base : se é a origem (sem  predecessor), o print
    imprimir_caminho(predecessor, predecessor[j]); // call recursivamente para print o pai direito
        printf("-%d", j); // Imprime o bairro atual separado pour um tracinho(formato : 1-3-2).
}

// FUNCIONALIDADE 2 : Calcule o mais curto caminho usando o algoritmo de Dijkstra
void planejar_rota(Cidade *c, int origem, int destino) {
    if (c->num_bairros == 0) { // Segurança.
        printf("Erro: Nenhum grafo foi carregado ainda. Use a opcao 0.\n");
        return;
    }
    if (origem < 1 || origem > c->num_bairros || destino < 1 || destino > c->num_bairros) { // Valida os limites
        printf("Erro: Bairro de origem ou destino invalido.\n");
        return;
    }
    printf("\n--- PLANEJAMENTO DE ROTA ---\n");
    int dist[MAX_BAIRROS], visitado[MAX_BAIRROS] = {0}, predecessor[MAX_BAIRROS]; // Tabela do algoritmo
    for (int i = 1; i <= c->num_bairros; i++) { dist[i] = INF; predecessor[i] = -1; } // Initializa as distancias ao infinito e os pais a -1
    dist[origem] = 0; // a distancia da origem até ela-mesma é nula
    
    for (int count = 0; count < c->num_bairros - 1; count++) { // Repete o processo para todos os nós
        int min = INF, u = -1; // Variaveis para buscar a distancia minima nao visitado
        for (int v = 1; v <= c->num_bairros; v++) {
            if (!visitado[v] && dist[v] <= min) { min = dist[v]; u = v; } // encontra o nó 'u' mais próximo
        }
        if (u == -1) break; // Se nenhum nó foi encontrado,os nós restantes sao inacessiveis
        visitado[u] = 1; // Marca o nó 'u' escolhido como tratado
        for (int v = 1; v <= c->num_bairros; v++) { // Update as distancias dos vizinhos de 'u'.
            // Se 'v' nao é tratado, que uma rua existe,e que passar pelo'u' oferece um caminhomais curto
            if (!visitado[v] && c->matriz[u][v] != INF && dist[u] != INF && dist[u] + c->matriz[u][v] < dist[v]) {
                dist[v] = dist[u] + c->matriz[u][v]; // Update a mais curta distancia como para 'v'
                predecessor[v] = u; // grava que o melhor caminho para atingir 'v' passa para 'u'.
            }
        }
    }
    if (dist[destino] == INF) { // Se a distancia final é sempre o infinito
        printf("Nao existe rota do bairro %d para o bairro %d\n", origem, destino); // os bairros sao desconectados
    } else {
        printf("Caminho: ");
        imprimir_caminho(predecessor, destino); // acciona o print recursivo da rota
        printf("\nCusto: %d\n", dist[destino]); // print a distancia total calculada.
    }
}

// FUNCIONALIDADE 3 : concepçao da infraestrutura minimal (Ciclovia) com o algoritmo de Prim
void expandir_infraestrutura(Cidade *c) {
    if (c->num_bairros == 0) { // Segurança.
        printf("Erro: Nenhum grafo foi carregado ainda. Use a opcao 0.\n");
        return;
    }
    printf("\n--- EXPANSAO DA INFRAESTRUTURA (CICLOVIA) ---\n");
    if (!verificar_conectividade_silencioso(c)) { // Regra de trabalho: a cidade deve ser 100% connectada
        printf("A cidade nao e conectada\n"); // Se ela é cortada em dois m a criaçao global é impossivel
        return;
    }
    int pai[MAX_BAIRROS], chave[MAX_BAIRROS], na_agm[MAX_BAIRROS] = {0}; // eStrutura para guardar a AGM
    for (int i = 1; i <= c->num_bairros; i++) chave[i] = INF; // Initializa os custos de ligaçao ao infinito
    chave[1] = 0; pai[1] = -1; // O primeiro nó serve de raiz (custo 0, nenhum pai)
    
    for (int count = 0; count < c->num_bairros - 1; count++) { // Repete para  conectar todos os nós nécessarios
        int min = INF, u = -1; // Variavel para buscar a aresta de menor custo a conectar
        for (int v = 1; v <= c->num_bairros; v++) {
            if (!na_agm[v] && chave[v] < min) { min = chave[v]; u = v; } // escolhe o nó fora de arvore a mais economica
        }
        if (u == -1) break;
        na_agm[u] = 1; // Inclui officialmente o nó 'u' dentro a struct da pista cyclavel
        for (int v = 1; v <= c->num_bairros; v++) { // Update dos custos de conecçao para os vizinhos de  'u'.
            // Se 'v' nao é ainda dentre da árvore e que a rua(u, v) é menos caro que a soluçao atual
            if (c->matriz[u][v] != INF && !na_agm[v] && c->matriz[u][v] < chave[v]) {
                pai[v] = u; // Defini 'u' como o raccordement ideal para 'v'.
                chave[v] = c->matriz[u][v]; // update o custo minimo desseb raccordement.
            }
        }
    }
    int custo_total = 0; // Variavel para acumular o custo financeiro total da infrastrutura
    for (int i = 2; i <= c->num_bairros; i++) { // Percorre a árvore para print as conexões mantidas.
        if (pai[i] != -1) { // Se o ńo tem um pai valido dentro da estrutura
            printf("(%d, %d) (%d)\n", pai[i], i, c->matriz[i][pai[i]]); // print a aresta (pai, filho) e seu peso
            custo_total += c->matriz[i][pai[i]]; // Adicione o peso dessa rua ao custo global
        }
    }
    printf("Custo total: %d\n", custo_total); // print o custo total da pista cyclavel
}

// MAIN : GESTAO DO MENU INTERATTIVO
int main() {
    Cidade c; // declara a instancia do objeto cidade
    inicializar_cidade(&c); // accione  a inicialisaçao da matriz em memoria
    
    int opcao = -1;     // Variavel para stock a escolha do menu feito pelo user
    char nome_arq[100]; // Variavel para stockear o nome textual do arquivo a carregar

    while (opcao != 4) { // loop principal : roda enquanto o user nao escolhe a opçao 4(Sair)
        printf("\n=========================================\n");
        printf("  SISTEMA DE MOBILIDADE URBANA - MENU    \n");
        printf("=========================================\n");
        printf("0. Carregar Arquivo de Grafo\n");
        printf("1. Analise de Conectividade e Alcance\n");
        printf("2. Planejamento de Rotas Inteligentes\n");
        printf("3. Expansao Economica da Infraestrutura\n");
        printf("4. Sair do Programa\n");
        printf("=========================================\n");
        // print dinamicamente se um arquivo é atualmente presente na memoria viva
        printf("Status atual: %s\n", (c.num_bairros > 0) ? "Grafo Carregado" : "Nenhum grafo na memoria");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) { // Lê o que o user escreveu. Se nao for um numero :
            printf("Entrada invalida!\n"); // Alerta error 
            while (getchar() != '\n'); // Esvazia o buffer de entrada para evitar uma loop inifinito sobre um texto residual
            continue; // acciona o menu 
        }

        int b_inicial, b_origem, b_destino; // Variaveis locais de transporte para as opçoes do menu
        switch (opcao) { // Estrutura de controle condicional baseada na escolha do user
            case 0: // Opçao carregar um arquivo
                printf("Digite o nome do arquivo : ");
                scanf("%s", nome_arq); // Capture o nome do arquivo escrito
                carregar_arquivo_grafo(&c, nome_arq); // call a funçao de carregamento dedicado
                break;
            case 1: // Opçao  Analise de conectividade.
                printf("Digite o bairro inicial para analise: ");
                scanf("%d", &b_inicial); // Capture o bairro de origem
                analisar_conectividade(&c, b_inicial); // accione o algoritmo DFS.
                break;
            case 2: // Opçao Dijkstra.
                printf("Digite o bairro de ORIGEM: ");
                scanf("%d", &b_origem); // Capture o ponto A.
                printf("Digite o bairro de DESTINO: ");
                scanf("%d", &b_destino); // Capture o ponto B.
                planejar_rota(&c, b_origem, b_destino); // accione o calculo do mais curto caminho
                break;
            case 3: // Opçao Prim.
                expandir_infraestrutura(&c); // accione a modelizaçao da arvore de custo minimo
                break;
            case 4: // Opçao sair.
                printf("Saindo do sistema... Ate logo!\n"); // Messagem de despedida.
                break;
            default: // todos as outras coisas que o user vai digitar e que nao sao referenciadas
                printf("Opcao invalida! Tente novamente.\n"); // Messagem de avisa
        }
    }

    return 0;
}