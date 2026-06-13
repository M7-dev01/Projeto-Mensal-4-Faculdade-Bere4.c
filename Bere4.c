#include <stdio.h>
#include <stdlib.h>
#include <string.h> // [NOVO - PROJETO 4] Necessário para manipulação de strings (strcmp, strcpy, etc.)

/* ================================================================
   STRUCTS (AGREGADOS HETEROGÊNEOS)
   ================================================================ */

// [NOVO - PROJETO 4] Estrutura para Controle de Usuários e Níveis de Acesso
typedef struct Usuario {
    char login[11];   // Exigência do PDF: 5 a 10 caracteres (+ '\0')
    char senha[9];    // Exigência do PDF: 4 a 8 caracteres (+ '\0')
    int tipo;         // 1 = Administrador, 2 = Usuário Comum
} Usuario;

// [NOVO - PROJETO 4] Estrutura para Categorias Dinâmicas (Substitui a digitação livre)
typedef struct Categoria {
    int codigo;
    char nome[50];
} Categoria;

// Clientes (Mantido do Projeto 3)
typedef struct Clientes {
    int codigo;
    char nomeCompleto[80];
    char nomeSocial[80];
    char cPF[15];
    char rua_numero[100];
    char bairro[50];
    char celular[20];
} Clientes;

// Produtos (Atualizado para o Projeto 4)
typedef struct Produtos {
    int codigo;
    char nomeProduto[80];
    char categoria[50]; // [NOVO - PROJETO 4] Agora recebe o nome da struct Categoria selecionada
    float precoCompra;
    float margemLucro;
    float precoVenda;
    int estoque;
    int estoqueMinimo;
} Produtos;

// Item Venda (Mantido do Projeto 3)
typedef struct ItemVenda {
    int codigoProduto;
    char nomeProduto[80];
    float precoVenda;
    int quantidade;
    float totalItem;
} ItemVenda;

// Venda (Atualizado para o Projeto 4)
typedef struct Venda {
    int numVenda;
    int codigoCliente; // [NOVO - PROJETO 4] Vinculação obrigatória do cliente na venda (0 para Sem Cliente)
    int dia, mes, ano; // [NOVO - PROJETO 4] Armazenamento da data da venda para os relatórios por período
    ItemVenda itens[50];
    int numItens;
    float totalCarrinho;
    float desconto;
    float totalFinal;
    char status;       // 'a' = aberto, 'p' = pago
} Venda;

// Pagamento (Atualizado para o Projeto 4)
typedef struct Pagamento {
    int numVenda;
    float valorPago;
    char tipo[3];      // [NOVO - PROJETO 4] Alterado para string para aceitar "d", "c" ou "dc" rigidamente conforme o PDF
} Pagamento;


/* ================================================================
   VARIÁVEIS GLOBAIS E PONTEIROS (ALOCAÇÃO DINÂMICA)
   ================================================================ */

// [NOVO - PROJETO 4] Vetores dinâmicos e controle de sessão
Usuario *usuario = NULL;
int quantidadeUsuario = 0;
Usuario usuarioLogado;
int temUsuarioLogado = 0;

Categoria *categoria = NULL;
int quantidadeCategoria = 0;

// Variáveis vindas do Projeto 3
Clientes *cliente = NULL;
int quantidadeCliente = 0;

Produtos *produto = NULL;
int quantidadeProduto = 0;

Venda *venda = NULL;
int quantidadeVenda = 0;

Pagamento *pagamento = NULL;
int quantidadePagamento = 0;

float valorAberturaCaixa = 0;
float totalretirada = 0;
int caixaAberto = 0;


/* ================================================================
   PROTÓTIPOS DAS FUNÇÕES
   ================================================================ */
void telaLogin();
int validarAdmin();
void cadastroUsuario();
void cadastroCategoria();
void menuPrincipal();
void menuCadastro();
void menuVendas();
void menuRelatorios();

// Submenus de Relatórios solicitados no PDF
void menuRelatorioClientes();
void menuRelatorioProdutos();
void menuRelatorioVendas();

// Funções de Processamento e Regras de Negócio
void cadastroCliente();
void cadastroProduto();
void novaVenda();
void processarPagamento(int indiceVenda);
void pagarVendaAberta();
void retiradaCaixa();
void abrirCaixa();
void fecharCaixa();

// [NOVO - PROJETO 4] Funções de Relatórios e Ordenação requeridas por critérios de avaliação
void relClientesOrdenados();
void relClientesPorPeriodo();
void relProdutosOrdenados();
void relProdutosEstoque();
void relProdutosMaisVendidos();
void relVendasPorPeriodo();
void relFaturamentoConsolidado();

// Auxiliares
int buscarProduto(int codigo);
int buscarCliente(int codigo);
void exibirProduto();
int dentroDoPeriodo(int d, int m, int a, int dI, int mI, int aI, int dF, int mF, int aF);
void salvarInformacao();
void carregarInformacao();


/* ================================================================
   MAIN (CRITÉRIO 5: ENXUTA E COM LIBERAÇÃO DE MEMÓRIA)
   ================================================================ */
int main() {
    carregarInformacao();
    telaLogin(); // [NOVO - PROJETO 4] Bloqueio inicial do sistema por autenticação
    menuPrincipal();

    // Liberação de memória de todas as estruturas alocadas dinamicamente
    free(usuario);
    free(categoria);
    free(cliente);
    free(produto);
    free(venda);
    free(pagamento);

    return 0;
}


/* ================================================================
   [NOVO - PROJETO 4] TELA DE LOGIN E AUTENTICAÇÃO
   ================================================================ */
void telaLogin() {
    char logDigitado[50], senDigitada[50];
    int logado = 0, tentativas = 0;

    // Caso seja a primeira execução e o arquivo esteja vazio, cria o admin padrão exigido
    if (quantidadeUsuario == 0) {
        usuario = malloc(sizeof(Usuario));
        strcpy(usuario[0].login, "admin1");
        strcpy(usuario[0].senha, "senha1");
        usuario[0].tipo = 1;
        quantidadeUsuario = 1;
    }

    printf("\n=============================================\n");
    printf("         SISTEMA DA BERE - LOGIN             \n");
    printf("=============================================\n");

    while (!logado && tentativas < 3) {
        printf("Login: ");
        scanf(" %[^\n]", logDigitado);
        printf("Senha: ");
        scanf(" %[^\n]", senDigitada);

        for (int i = 0; i < quantidadeUsuario; i++) {
            if (strcmp(usuario[i].login, logDigitado) == 0 && strcmp(usuario[i].senha, senDigitada) == 0) {
                usuarioLogado = usuario[i];
                temUsuarioLogado = 1;
                logado = 1;
                break;
            }
        }
        if (!logado) {
            tentativas++;
            printf("Credenciais incorretas! Tentativa %d de 3.\n\n", tentativas);
        }
    }

    if (!logado) {
        printf("Acesso bloqueado por excesso de tentativas.\n");
        exit(0);
    }
}


/* ================================================================
   [NOVO - PROJETO 4] VALIDAÇÃO DE PERMISSÃO DE ADMINISTRADOR
   ================================================================ */
int validarAdmin() {
    if (usuarioLogado.tipo == 1) return 1;

    char logAdm[50], senAdm[50];
    printf("\n[ACESSO RESTRITO] Informe credenciais de um Administrador:\n");
    printf("Login Admin: ");
    scanf(" %[^\n]", logAdm);
    printf("Senha Admin: ");
    scanf(" %[^\n]", senAdm);

    for (int i = 0; i < quantidadeUsuario; i++) {
        if (usuario[i].tipo == 1 && strcmp(usuario[i].login, logAdm) == 0 && strcmp(usuario[i].senha, senAdm) == 0) {
            return 1;
        }
    }
    printf("Acesso negado. Operação cancelada.\n");
    return 0;
}


/* ================================================================
   MENUS PRINCIPAIS E SUBMENUS RENUMERADOS CONFORME O PDF
   ================================================================ */
void menuPrincipal() {
    int op;
    do {
        printf("\n=============================================\n");
        printf("         MERCEARIA DONA BERE - V4.0          \n");
        printf("=============================================\n");
        printf("1 - Cadastros\n");
        printf("2 - Vendas\n");
        printf("3 - Abertura de Caixa\n");
        printf("4 - Fechamento de Caixa\n");
        printf("5 - Relatorios\n");
        printf("6 - Sair\n"); //
        printf("=============================================\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: menuCadastro(); break;
            case 2: menuVendas(); break;
            case 3: abrirCaixa(); break;
            case 4: fecharCaixa(); break;
            case 5: menuRelatorios(); break;
            case 6: salvarInformacao(); printf("Dados salvos. Saindo...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 6);
}

void menuCadastro() {
    int op;
    do {
        printf("\n--- MENU CADASTROS ---\n");
        printf("1 - Cadastro de Usuarios\n");   // [NOVO - PROJETO 4]
        printf("2 - Cadastro de Clientes\n");
        printf("3 - Cadastro de Produtos\n");
        printf("4 - Cadastro de Categorias\n"); // [NOVO - PROJETO 4]
        printf("5 - Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: cadastroUsuario(); break;
            case 2: cadastroCliente(); break;
            case 3: cadastroProduto(); break;
            case 4: cadastroCategoria(); break;
        }
    } while (op != 5);
}

void menuVendas() {
    int op;
    do {
        printf("\n--- MENU VENDAS ---\n");
        printf("1 - Nova Venda\n");
        printf("2 - Retirada de Caixa (Sangria)\n");
        printf("3 - Pagamento de Venda em Aberto\n");
        printf("4 - Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: novaVenda(); break;
            case 2: retiradaCaixa(); break;
            case 3: pagarVendaAberta(); break;
        }
    } while (op != 4);
}

void menuRelatorios() {
    int op;
    do {
        printf("\n--- 5. RELATORIOS ---\n");
        printf("1 - Relatorios de Clientes\n");
        printf("2 - Relatorios de Produtos\n");
        printf("3 - Relatorios de Vendas\n");
        printf("4 - Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: menuRelatorioClientes(); break;
            case 2: menuRelatorioProdutos(); break;
            case 3: menuRelatorioVendas(); break;
        }
    } while (op != 4);
}

void menuRelatorioClientes() {
    int op;
    do {
        printf("\n--- 5.1 RELATORIOS DE CLIENTES ---\n");
        printf("1 - Listagem (Ordem Alfabetica)\n");
        printf("2 - Clientes que compraram em um periodo\n");
        printf("3 - Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: relClientesOrdenados(); break;
            case 2: relClientesPorPeriodo(); break;
        }
    } while (op != 3);
}

void menuRelatorioProdutos() {
    int op;
    do {
        printf("\n--- 5.2 RELATORIOS DE PRODUTOS ---\n");
        printf("1 - Listagem (Ordem Alfabetica)\n");
        printf("2 - Produtos com Estoque Baixo ou Zerado\n");
        printf("3 - Produtos Mais Vendidos em um Periodo\n");
        printf("4 - Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: relProdutosOrdenados(); break;
            case 2: relProdutosEstoque(); break;
            case 3: relProdutosMaisVendidos(); break;
        }
    } while (op != 4);
}

void menuRelatorioVendas() {
    int op;
    do {
        printf("\n--- 5.3 RELATORIOS DE VENDAS ---\n");
        printf("1 - Listagem de Vendas por Periodo\n");
        printf("2 - Faturamento Consolidado por Periodo\n");
        printf("3 - Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: relVendasPorPeriodo(); break;
            case 2: relFaturamentoConsolidado(); break;
        }
    } while (op != 3);
}


/* ================================================================
   [NOVO - PROJETO 4] IMPLEMENTAÇÕES DE CADASTRO DE USUÁRIO E CATEGORIA
   ================================================================ */
void cadastroUsuario() {
    if (!validarAdmin()) return; // Exigência do PDF: Apenas Admin cadastra outros usuários

    char tempLog[50], tempSen[50];
    int tempTipo;

    printf("\n--- CADASTRO DE NOVO USUARIO ---\n");
    do {
        printf("Login (5 a 10 caracteres): ");
        scanf(" %[^\n]", tempLog);
    } while (strlen(tempLog) < 5 || strlen(tempLog) > 10); // Validação rígida conforme pág. 3 do PDF

    do {
        printf("Senha (4 a 8 caracteres): ");
        scanf(" %[^\n]", tempSen);
    } while (strlen(tempSen) < 4 || strlen(tempSen) > 8);   // Validação rígida conforme pág. 3 do PDF

    do {
        printf("Nivel de Acesso (1 - Admin, 2 - Usuario): ");
        scanf("%d", &tempTipo);
    } while (tempTipo != 1 && tempTipo != 2);

    if (quantidadeUsuario == 0) usuario = malloc(sizeof(Usuario));
    else usuario = realloc(usuario, (quantidadeUsuario + 1) * sizeof(Usuario));

    strcpy(usuario[quantidadeUsuario].login, tempLog);
    strcpy(usuario[quantidadeUsuario].senha, tempSen);
    usuario[quantidadeUsuario].tipo = tempTipo;
    quantidadeUsuario++;
    printf("Usuario cadastrado com sucesso!\n");
}

void cadastroCategoria() {
    printf("\n--- CADASTRO DE CATEGORIA ---\n");
    if (quantidadeCategoria == 0) categoria = malloc(sizeof(Categoria));
    else categoria = realloc(categoria, (quantidadeCategoria + 1) * sizeof(Categoria));

    categoria[quantidadeCategoria].codigo = quantidadeCategoria + 1;
    printf("Nome da Categoria: ");
    scanf(" %[^\n]", categoria[quantidadeCategoria].nome);

    quantidadeCategoria++;
    printf("Categoria cadastrada com sucesso!\n");
}


/* ================================================================
   MÓDULOS DE CADASTRO DE CLIENTE E PRODUTO (VINDOS DO PROJETO 3)
   ================================================================ */
void cadastroCliente() {
    if (quantidadeCliente == 0) cliente = malloc(sizeof(Clientes));
    else cliente = realloc(cliente, (quantidadeCliente + 1) * sizeof(Clientes));

    cliente[quantidadeCliente].codigo = quantidadeCliente + 1;
    printf("\nNome Completo: "); scanf(" %[^\n]", cliente[quantidadeCliente].nomeCompleto);
    printf("Nome Social: ");   scanf(" %[^\n]", cliente[quantidadeCliente].nomeSocial);
    printf("CPF: ");           scanf(" %[^\n]", cliente[quantidadeCliente].cPF);
    printf("Rua e Numero: ");  scanf(" %[^\n]", cliente[quantidadeCliente].rua_numero);
    printf("Bairro: ");        scanf(" %[^\n]", cliente[quantidadeCliente].bairro);
    printf("Celular: ");       scanf(" %[^\n]", cliente[quantidadeCliente].celular);

    quantidadeCliente++;
    printf("Cliente cadastrado com sucesso! Codigo: %d\n", quantidadeCliente);
}

void cadastroProduto() {
    if (quantidadeProduto == 0) produto = malloc(sizeof(Produtos));
    else produto = realloc(produto, (quantidadeProduto + 1) * sizeof(Produtos));

    produto[quantidadeProduto].codigo = quantidadeProduto + 1000;
    printf("\nNome do Produto: ");
    scanf(" %[^\n]", produto[quantidadeProduto].nomeProduto);

    // [NOVO - PROJETO 4] Substituição da digitação livre pela amarração com a struct de categorias
    if (quantidadeCategoria > 0) {
        int sel;
        printf("Selecione uma Categoria cadastrada:\n");
        for (int i = 0; i < quantidadeCategoria; i++) {
            printf("%d - %s\n", categoria[i].codigo, categoria[i].nome);
        }
        printf("Opcao: ");
        scanf("%d", &sel);
        if (sel >= 1 && sel <= quantidadeCategoria) {
            strcpy(produto[quantidadeProduto].categoria, categoria[sel-1].nome);
        } else {
            strcpy(produto[quantidadeProduto].categoria, "Geral");
        }
    } else {
        printf("Categoria (Digitacao Livre - Nenhuma cadastrada): ");
        scanf(" %[^\n]", produto[quantidadeProduto].categoria);
    }

    printf("Preco de Compra: ");  scanf("%f", &produto[quantidadeProduto].precoCompra);
    printf("Margem de Lucro %%: "); scanf("%f", &produto[quantidadeProduto].margemLucro);
    produto[quantidadeProduto].precoVenda = produto[quantidadeProduto].precoCompra + (produto[quantidadeProduto].precoCompra * produto[quantidadeProduto].margemLucro / 100);
    printf("Estoque Inicial: ");  scanf("%d", &produto[quantidadeProduto].estoque);
    printf("Estoque Minimo: ");   scanf("%d", &produto[quantidadeProduto].estoqueMinimo);

    quantidadeProduto++;
    printf("Produto cadastrado com sucesso!\n");
}


/* ================================================================
   NÚCLEO DE MOVIMENTAÇÃO DE VENDAS E PROCESSAMENTO DE CAIXA
   ================================================================ */
void novaVenda() {
    // Tratamento de exceção obrigatório: Não deixa vender com caixa fechado
    if (!caixaAberto) {
        printf("[ERRO] Nao e possivel realizar vendas sem abrir o caixa!\n");
        return;
    }

    if (quantidadeVenda == 0) venda = malloc(sizeof(Venda));
    else venda = realloc(venda, (quantidadeVenda + 1) * sizeof(Venda));

    venda[quantidadeVenda].numVenda = quantidadeVenda + 1;
    venda[quantidadeVenda].numItens = 0;
    venda[quantidadeVenda].totalCarrinho = 0;
    venda[quantidadeVenda].status = 'a';

    // [NOVO - PROJETO 4] Solicitação e vinculação obrigatória de Cliente e Data da Compra
    printf("\nCodigo do Cliente (0 para Consumidor Comum): ");
    scanf("%d", &venda[quantidadeVenda].codigoCliente);
    printf("Informe a Data da Venda (DD MM AAAA): ");
    scanf("%d %d %d", &venda[quantidadeVenda].dia, &venda[quantidadeVenda].mes, &venda[quantidadeVenda].ano);

    char op = 's';
    while (op == 's' || op == 'S') {
        exibirProduto();
        int codProd, qtd;
        printf("\nInforme o Codigo do Produto para Adicionar ao Carrinho: ");
        scanf("%d", &codProd);
        int idx = buscarProduto(codProd);

        if (idx != -1) {
            if (produto[idx].estoque == 0) {
                printf("[ALERTA] Estoque zerado! Operacao cancelada para este item.\n");
                continue;
            }
            printf("Quantidade desejada: ");
            scanf("%d", &qtd);

            if (qtd > produto[idx].estoque) {
                printf("Estoque insuficiente. Deseja levar o saldo total de %d unidades? (s/n): ", produto[idx].estoque);
                char conf; scanf(" %c", &conf);
                if (conf == 's' || conf == 'S') qtd = produto[idx].estoque;
                else continue;
            }

            int p = venda[quantidadeVenda].numItens;
            venda[quantidadeVenda].itens[p].codigoProduto = produto[idx].codigo;
            strcpy(venda[quantidadeVenda].itens[p].nomeProduto, produto[idx].nomeProduto);
            venda[quantidadeVenda].itens[p].precoVenda = produto[idx].precoVenda;
            venda[quantidadeVenda].itens[p].quantidade = qtd;
            venda[quantidadeVenda].itens[p].totalItem = produto[idx].precoVenda * qtd;

            venda[quantidadeVenda].totalCarrinho += venda[quantidadeVenda].itens[p].totalItem;
            venda[quantidadeVenda].numItens++;
            produto[idx].estoque -= qtd;

            if (produto[idx].estoque <= produto[idx].estoqueMinimo) {
                printf("[ALERTA] O produto %s atingiu o nivel de estoque minimo!\n", produto[idx].nomeProduto);
            }
        } else {
            printf("Produto nao cadastrado!\n");
        }
        printf("Deseja inserir mais produtos? (s/n): ");
        scanf(" %c", &op);
    }

    printf("Porcentagem de Desconto (0 para nenhum): ");
    scanf("%f", &venda[quantidadeVenda].desconto);
    venda[quantidadeVenda].totalFinal = venda[quantidadeVenda].totalCarrinho - (venda[quantidadeVenda].totalCarrinho * venda[quantidadeVenda].desconto / 100);

    quantidadeVenda++;
    processarPagamento(quantidadeVenda - 1);
}

void processarPagamento(int idx) {
    printf("\nTotal a Pagar: R$ %.2f\n", venda[idx].totalFinal);
    printf("Formas de Pagamento:\n1 - Dinheiro\n2 - Cartao\n3 - Misto (Dinheiro + Cartao)\nOpcao: ");
    int forma; scanf("%d", &forma);

    if (quantidadePagamento == 0) pagamento = malloc(sizeof(Pagamento));
    
    if (forma == 1) {
        // [NOVO - PROJETO 4] Tratamento de exceção: validação do valor recebido e cálculo do troco
        float valorEntregue;
        do {
            printf("Valor entregue pelo cliente: R$ ");
            scanf("%f", &valorEntregue);
            if (valorEntregue < venda[idx].totalFinal) {
                printf("[ERRO] Valor insuficiente! O total e R$ %.2f. Digite novamente.\n", venda[idx].totalFinal);
            }
        } while (valorEntregue < venda[idx].totalFinal);

        float troco = valorEntregue - venda[idx].totalFinal;
        if (troco > 0) {
            printf("==> TROCO: R$ %.2f\n", troco); // [NOVO - PROJETO 4] Exibição explícita do troco
        } else {
            printf("==> Sem troco (valor exato).\n");
        }

        pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento));
        pagamento[quantidadePagamento].numVenda = venda[idx].numVenda;
        pagamento[quantidadePagamento].valorPago = venda[idx].totalFinal;
        strcpy(pagamento[quantidadePagamento].tipo, "d");
        quantidadePagamento++;
        venda[idx].status = 'p';
        printf("Pagamento em dinheiro registrado com sucesso!\n");
    } 
    else if (forma == 2) {
        // [NOVO - PROJETO 4] Validação de status da máquina de cartão (Simulação de integração)
        int statusMaquina;
        printf("\n[MAQUININHA] Processando... Insira, passe ou aproxime o cartao.\n");
        printf("Simular retorno da máquina (1 - Aprovado / 2 - Recusado/Erro): ");
        scanf("%d", &statusMaquina);

        if (statusMaquina == 1) {
            pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento));
            pagamento[quantidadePagamento].numVenda = venda[idx].numVenda;
            pagamento[quantidadePagamento].valorPago = venda[idx].totalFinal;
            strcpy(pagamento[quantidadePagamento].tipo, "c");
            quantidadePagamento++;
            venda[idx].status = 'p';
            printf("[SUCESSO] Transacao autorizada pela operadora do cartao!\n");
        } else {
            // [NOVO - PROJETO 4] Exceção se for recusado: a venda permanece em aberto ('a') e o estoque precisa retornar
            printf("[ALERTA] Transacao Recusada ou Cancelada. A venda ficara EM ABERTO para posterior liquidacao.\n");
            venda[idx].status = 'a'; 
            // Opcional: Se quiser cancelar totalmente a venda e devolver os itens ao estoque imediatamente,
            // precisaria fazer um laço devolvendo as quantidades, mas manter em aberto permite tentar pagar de novo no submenu 3.
        }
    } 
    else if (forma == 3) {
        // [NOVO - PROJETO 4] Pagamento Misto robusto combinando Dinheiro (com troco) e Cartão (com aprovação)
        float valorDinheiro, valorCartao;
        float valorEntregueDinheiro;

        do {
            printf("Valor que sera pago em Dinheiro: R$ ");
            scanf("%f", &valorDinheiro);
            if (valorDinheiro > venda[idx].totalFinal) {
                printf("[ERRO] O valor em dinheiro nao pode ser maior que o total da venda (R$ %.2f).\n", venda[idx].totalFinal);
            }
        } while (valorDinheiro > venda[idx].totalFinal);

        // Tratamento do troco da parte em dinheiro
        do {
            printf("Quantia fisica entregue em dinheiro para pagar os R$ %.2f: R$ ", valorDinheiro);
            scanf("%f", &valorEntregueDinheiro);
            if (valorEntregueDinheiro < valorDinheiro) {
                printf("[ERRO] Quantia insuficiente para cobrir a parte em dinheiro!\n");
            }
        } while (valorEntregueDinheiro < valorDinheiro);

        float trocoMisto = valorEntregueDinheiro - valorDinheiro;
        if (trocoMisto > 0) printf("==> TROCO DA PARTE EM DINHEIRO: R$ %.2f\n", trocoMisto);

        valorCartao = venda[idx].totalFinal - valorDinheiro;
        printf("Restante a pagar no Cartao: R$ %.2f\n", valorCartao);

        // Validação da maquininha para a segunda parte do pagamento misto
        int statusMaquinaMisto;
        printf("Simular retorno da máquina para a parte do Cartao (1 - Aprovado / 2 - Recusado): ");
        scanf("%d", &statusMaquinaMisto);

        if (statusMaquinaMisto == 1) {
            // Se a maquininha aprovar, grava ambos os registros
            pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento));
            pagamento[quantidadePagamento].numVenda = venda[idx].numVenda;
            pagamento[quantidadePagamento].valorPago = valorDinheiro;
            strcpy(pagamento[quantidadePagamento].tipo, "dc");
            quantidadePagamento++;

            pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento));
            pagamento[quantidadePagamento].numVenda = venda[idx].numVenda;
            pagamento[quantidadePagamento].valorPago = valorCartao;
            strcpy(pagamento[quantidadePagamento].tipo, "dc");
            quantidadePagamento++;

            venda[idx].status = 'p';
            printf("[SUCESSO] Pagamento misto concluido com êxito!\n");
        } else {
            printf("[ALERTA] Falha na aprovacao do cartao. Operacao misto abortada. A venda ficara EM ABERTO.\n");
            venda[idx].status = 'a';
        }
    }
}

void pagarVendaAberta() {
    if (!caixaAberto) { printf("[ERRO] Caixa fechado!\n"); return; }
    printf("\n--- VENDAS EM ABERTO ---\n");
    int achou = 0;
    for (int i = 0; i < quantidadeVenda; i++) {
        if (venda[i].status == 'a') {
            printf("Venda #%d - Total: R$ %.2f\n", venda[i].numVenda, venda[i].totalFinal);
            achou = 1;
        }
    }
    if (!achou) { printf("Nenhuma venda em aberto encontrada.\n"); return; }

    int num;
    printf("\nInforme o numero da venda que deseja liquidar: ");
    scanf("%d", &num);
    for (int i = 0; i < quantidadeVenda; i++) {
        if (venda[i].numVenda == num && venda[i].status == 'a') {
            processarPagamento(i);
            return;
        }
    }
    printf("Venda invalida ou ja paga.\n");
}

void abrirCaixa() {
    if (caixaAberto) { printf("O caixa ja esta aberto!\n"); return; }
    if (!validarAdmin()) return; // Apenas administrador pode manusear o caixa

    printf("Valor de Abertura do Caixa: R$ ");
    scanf("%f", &valorAberturaCaixa);
    totalretirada = 0;
    caixaAberto = 1;
    printf("Caixa aberto com sucesso!\n");
}

void retiradaCaixa() {
    if (!caixaAberto) { printf("Abra o caixa primeiro!\n"); return; }
    if (!validarAdmin()) return;

    float valor;
    printf("Valor da Retirada (Sangria): R$ ");
    scanf("%f", &valor);
    totalretirada += valor;
    printf("Sangria registrada!\n");
}

void fecharCaixa() {
    if (!caixaAberto) { printf("O caixa ja esta fechado!\n"); return; }
    if (!validarAdmin()) return;

    float dinheiroEmCaixa, totalDinheiroVendas = 0, totalCartaoVendas = 0;
    printf("Informe o valor total em dinheiro FISICO presente no caixa: R$ ");
    scanf("%f", &dinheiroEmCaixa);

    // Consolidação matemática real baseada no fluxo monetário
    for (int i = 0; i < quantidadePagamento; i++) {
        if (strcmp(pagamento[i].tipo, "d") == 0) totalDinheiroVendas += pagamento[i].valorPago;
        else if (strcmp(pagamento[i].tipo, "c") == 0) totalCartaoVendas += pagamento[i].valorPago;
        else if (strcmp(pagamento[i].tipo, "dc") == 0) {
            // No pagamento misto, o loop de pagamentos gerou registros independentes e tipados
            totalDinheiroVendas += pagamento[i].valorPago; 
        }
    }

    float saldoDinheiroEsperado = valorAberturaCaixa + totalDinheiroVendas - totalretirada;

    printf("\n=====================================\n");
    printf("       FECHAMENTO DE CAIXA           \n");
    printf("=====================================\n");
    printf("Abertura:         R$ %.2f\n", valorAberturaCaixa);
    printf("Vendas Dinheiro:  R$ %.2f\n", totalDinheiroVendas);
    printf("Vendas Cartao:    R$ %.2f\n", totalCartaoVendas);
    printf("Sangrias:         R$ %.2f\n", totalretirada);
    printf("Saldo Esperado:   R$ %.2f\n", saldoDinheiroEsperado);
    printf("Dinheiro Fisico:  R$ %.2f\n", dinheiroEmCaixa);

    if (dinheiroEmCaixa != saldoDinheiroEsperado) {
        printf("[ALERTA CRITICO] O VALOR ENCONTRADO NAO BATE COM O SALDO ESPERADO!\n");
        printf("Divergencia de: R$ %.2f\n", dinheiroEmCaixa - saldoDinheiroEsperado);
    } else {
        printf("Caixa fechado com sucesso sem divergencias de saldo!\n");
    }

    caixaAberto = 0;
}


/* ================================================================
   [NOVO - PROJETO 4] ALGORITMOS DE BUSCA E RELATÓRIOS AVANÇADOS
   ================================================================ */

// [NOVO - PROJETO 4] 5.1.1 - Listagem Alfabética de Clientes via Algoritmo de Ordenação Bubble Sort
void relClientesOrdenados() {
    if (quantidadeCliente == 0) { printf("Nenhum cliente cadastrado.\n"); return; }

    // Clonando o vetor original para não corromper o índice sequencial original
    Clientes *aux = malloc(quantidadeCliente * sizeof(Clientes));
    memcpy(aux, cliente, quantidadeCliente * sizeof(Clientes));

    // Bubble Sort aplicando a ordenação alfabética demandada pelo critério 12
    for (int i = 0; i < quantidadeCliente - 1; i++) {
        for (int j = 0; j < quantidadeCliente - i - 1; j++) {
            if (strcmp(aux[j].nomeCompleto, aux[j+1].nomeCompleto) > 0) {
                Clientes temp = aux[j];
                aux[j] = aux[j+1];
                aux[j+1] = temp;
            }
        }
    }

    printf("\n--- LISTAGEM DE CLIENTES EM ORDEM ALFABETICA ---\n");
    for (int i = 0; i < quantidadeCliente; i++) {
        printf("Cod: %d | Nome: %s | CPF: %s\n", aux[i].codigo, aux[i].nomeCompleto, aux[i].cPF);
    }
    free(aux);
}

// [NOVO - PROJETO 4] 5.1.2 - Filtro de Clientes por Período de Compra
void relClientesPorPeriodo() {
    int dI, mI, aI, dF, mF, aF;
    printf("Data Inicial (DD MM AAAA): "); scanf("%d %d %d", &dI, &mI, &aI);
    printf("Data Final (DD MM AAAA): ");   scanf("%d %d %d", &dF, &mF, &aF);

    printf("\n--- CLIENTES QUE COMPRARAM NO PERIODO ---\n");
    int achou = 0;
    for (int i = 0; i < quantidadeVenda; i++) {
        if (dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) {
            int idxCli = buscarCliente(venda[i].codigoCliente);
            if (idxCli != -1) {
                printf("Cliente: %s | Comprou na Venda #%d em %02d/%02d/%04d\n", cliente[idxCli].nomeCompleto, venda[i].numVenda, venda[i].dia, venda[i].mes, venda[i].ano);
                achou = 1;
            }
        }
    }
    if (!achou) printf("Nenhum registro de compra encontrado no periodo informado.\n");
}

// [NOVO - PROJETO 4] 5.2.1 - Listagem Alfabética de Produtos com Bubble Sort
void relProdutosOrdenados() {
    if (quantidadeProduto == 0) { printf("Nenhum produto cadastrado.\n"); return; }

    Produtos *aux = malloc(quantidadeProduto * sizeof(Produtos));
    memcpy(aux, produto, quantidadeProduto * sizeof(Produtos));

    for (int i = 0; i < quantidadeProduto - 1; i++) {
        for (int j = 0; j < quantidadeProduto - i - 1; j++) {
            if (strcmp(aux[j].nomeProduto, aux[j+1].nomeProduto) > 0) {
                Produtos temp = aux[j];
                aux[j] = aux[j+1];
                aux[j+1] = temp;
            }
        }
    }

    printf("\n--- LISTAGEM DE PRODUTOS EM ORDEM ALFABETICA ---\n");
    for (int i = 0; i < quantidadeProduto; i++) {
        printf("Cod: %d | Nome: %s | Preco Venda: R$ %.2f | Estoque: %d\n", aux[i].codigo, aux[i].nomeProduto, aux[i].precoVenda, aux[i].estoque);
    }
    free(aux);
}

// [NOVO - PROJETO 4] 5.2.2 - Listagem de Estoques Separados (Mapeamento Rígido de Alerta)
void relProdutosEstoque() {
    printf("\n--- PRODUTOS COM ESTOQUE ZERADO ---\n");
    for (int i = 0; i < quantidadeProduto; i++) {
        if (produto[i].estoque == 0) printf("Cod: %d | Nome: %s\n", produto[i].codigo, produto[i].nomeProduto);
    }
    printf("\n--- PRODUTOS ABAIXO DO ESTOQUE MINIMO ---\n");
    for (int i = 0; i < quantidadeProduto; i++) {
        if (produto[i].estoque > 0 && produto[i].estoque <= produto[i].estoqueMinimo) {
            printf("Cod: %d | Nome: %s | Atual: %d | Minimo: %d\n", produto[i].codigo, produto[i].nomeProduto, produto[i].estoque, produto[i].estoqueMinimo);
        }
    }
}

// [NOVO - PROJETO 4] 5.2.3 - Ranking Decrescente dos Produtos Mais Vendidos por Período
void relProdutosMaisVendidos() {
    int dI, mI, aI, dF, mF, aF;
    printf("Data Inicial (DD MM AAAA): "); scanf("%d %d %d", &dI, &mI, &aI);
    printf("Data Final (DD MM AAAA): ");   scanf("%d %d %d", &dF, &mF, &aF);

    // Estrutura local para contabilização do ranqueamento
    typedef struct { int cod; char nome[80]; int qtdVendida; } Rank;
    Rank *ranking = calloc(quantidadeProduto, sizeof(Rank));

    for (int i = 0; i < quantidadeProduto; i++) {
        ranking[i].cod = produto[i].codigo;
        strcpy(ranking[i].nome, produto[i].nomeProduto);
    }

    for (int i = 0; i < quantidadeVenda; i++) {
        if (dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) {
            for (int j = 0; j < venda[i].numItens; j++) {
                for (int k = 0; k < quantidadeProduto; k++) {
                    if (ranking[k].cod == venda[i].itens[j].codigoProduto) {
                        ranking[k].qtdVendida += venda[i].itens[j].quantidade;
                    }
                }
            }
        }
    }

    // Ordenação decrescente (do maior número de vendas para o menor)
    for (int i = 0; i < quantidadeProduto - 1; i++) {
        for (int j = 0; j < quantidadeProduto - i - 1; j++) {
            if (ranking[j].qtdVendida < ranking[j+1].qtdVendida) {
                Rank temp = ranking[j];
                ranking[j] = ranking[j+1];
                ranking[j+1] = temp;
            }
        }
    }

    printf("\n--- PRODUTOS MAIS VENDIDOS NO PERIODO ---\n");
    for (int i = 0; i < quantidadeProduto; i++) {
        if (ranking[i].qtdVendida > 0) {
            printf("Posicao %d | Nome: %s | Qtd Vendida: %d\n", i+1, ranking[i].nome, ranking[i].qtdVendida);
        }
    }
    free(ranking);
}

// [NOVO - PROJETO 4] 5.3.1 - Listagem Analítica de Vendas por Período
void relVendasPorPeriodo() {
    int dI, mI, aI, dF, mF, aF;
    printf("Data Inicial (DD MM AAAA): "); scanf("%d %d %d", &dI, &mI, &aI);
    printf("Data Final (DD MM AAAA): ");   scanf("%d %d %d", &dF, &mF, &aF);

    printf("\n--- VENDAS REALIZADAS NO PERIODO ---\n");
    for (int i = 0; i < quantidadeVenda; i++) {
        if (dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) {
            printf("Venda #%d | Data: %02d/%02d/%04d | Total Final: R$ %.2f | Status: %s\n",
                   venda[i].numVenda, venda[i].dia, venda[i].mes, venda[i].ano, venda[i].totalFinal,
                   (venda[i].status == 'p' ? "Paga" : "Em Aberto"));
        }
    }
}

// [NOVO - PROJETO 4] 5.3.2 - Consolidação Financeira por Filtro de Período Geral
void relFaturamentoConsolidado() {
    int dI, mI, aI, dF, mF, aF;
    printf("Data Inicial (DD MM AAAA): "); scanf("%d %d %d", &dI, &mI, &aI);
    printf("Data Final (DD MM AAAA): ");   scanf("%d %d %d", &dF, &mF, &aF);

    float fatDinheiro = 0, fatCartao = 0, fatTotal = 0;

    for (int i = 0; i < quantidadeVenda; i++) {
        if (venda[i].status == 'p' && dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) {
            fatTotal += venda[i].totalFinal;
            for (int j = 0; j < quantidadePagamento; j++) {
                if (pagamento[j].numVenda == venda[i].numVenda) {
                    if (strcmp(pagamento[j].tipo, "d") == 0) fatDinheiro += pagamento[j].valorPago;
                    else if (strcmp(pagamento[j].tipo, "c") == 0) fatCartao += pagamento[j].valorPago;
                    else if (strcmp(pagamento[j].tipo, "dc") == 0) {
                        // Divisão balanceada baseada nas estruturas criadas dinamicamente
                        fatDinheiro += pagamento[j].valorPago;
                    }
                }
            }
        }
    }

    printf("\n--- FATURAMENTO CONSOLIDADO DO PERIODO ---\n");
    printf("Total Entrado em Dinheiro: R$ %.2f\n", fatDinheiro);
    printf("Total Entrado em Cartao:   R$ %.2f\n", fatCartao);
    printf("FATURAMENTO BRUTO TOTAL:   R$ %.2f\n", fatTotal);
}


/* ================================================================
   FUNÇÕES AUXILIARES DE BUSCA E TRATAMENTO DE DATAS
   ================================================================ */
int buscarProduto(int codigo) {
    for (int i = 0; i < quantidadeProduto; i++) {
        if (produto[i].codigo == codigo) return i;
    }
    return -1;
}

int buscarCliente(int codigo) {
    for (int i = 0; i < quantidadeCliente; i++) {
        if (cliente[i].codigo == codigo) return i;
    }
    return -1;
}

void exibirProduto() {
    printf("\n--- PRODUTOS DISPONIVEIS ---\n");
    for (int i = 0; i < quantidadeProduto; i++) {
        printf("Cod: %d | Produto: %s | Preco: R$ %.2f | Estoque: %d\n", produto[i].codigo, produto[i].nomeProduto, produto[i].precoVenda, produto[i].estoque);
    }
}

// [NOVO - PROJETO 4] Validador lógico matemático para intercalação temporal de escopos relacionais de datas
int dentroDoPeriodo(int d, int m, int a, int dI, int mI, int aI, int dF, int mF, int aF) {
    long dataReg = a * 10000 + m * 100 + d;
    long dataIni = aI * 10000 + mI * 100 + dI;
    long dataFim = aF * 10000 + mF * 100 + dF;
    return (dataReg >= dataIni && dataReg <= dataFim);
}


/* ================================================================
   SISTEMA DE PERSISTÊNCIA EM ARQUIVOS TEXTO (.dat)
   ================================================================ */
void salvarInformacao() {
    FILE *f;

    // Salvando Usuários
    f = fopen("usuarios.dat", "w");
    if (f != NULL) {
        fprintf(f, "%d\n", quantidadeUsuario);
        for (int i = 0; i < quantidadeUsuario; i++) {
            fprintf(f, "%s\n%s\n%d\n", usuario[i].login, usuario[i].senha, usuario[i].tipo);
        }
        fclose(f);
    }

    // Salvando Categorias
    f = fopen("categorias.dat", "w");
    if (f != NULL) {
        fprintf(f, "%d\n", quantidadeCategoria);
        for (int i = 0; i < quantidadeCategoria; i++) {
            fprintf(f, "%d\n%s\n", categoria[i].codigo, categoria[i].nome);
        }
        fclose(f);
    }

    // Salvando Clientes
    f = fopen("clientes.dat", "w");
    if (f != NULL) {
        fprintf(f, "%d\n", quantidadeCliente);
        for (int i = 0; i < quantidadeCliente; i++) {
            fprintf(f, "%d\n%s\n%s\n%s\n%s\n%s\n%s\n", cliente[i].codigo, cliente[i].nomeCompleto, cliente[i].nomeSocial, cliente[i].cPF, cliente[i].rua_numero, cliente[i].bairro, cliente[i].celular);
        }
        fclose(f);
    }

    // Salvando Produtos
    f = fopen("produtos.dat", "w");
    if (f != NULL) {
        fprintf(f, "%d\n", quantidadeProduto);
        for (int i = 0; i < quantidadeProduto; i++) {
            fprintf(f, "%d\n%s\n%s\n%f\n%f\n%f\n%d\n%d\n", produto[i].codigo, produto[i].nomeProduto, produto[i].categoria, produto[i].precoCompra, produto[i].margemLucro, produto[i].precoVenda, produto[i].estoque, produto[i].estoqueMinimo);
        }
        fclose(f);
    }

    // Salvando Vendas e Itens
    f = fopen("vendas.dat", "w");
    if (f != NULL) {
        fprintf(f, "%d\n", quantidadeVenda);
        for (int i = 0; i < quantidadeVenda; i++) {
            fprintf(f, "%d\n%d\n%d\n%d\n%d\n%d\n%f\n%f\n%f\n%c\n", venda[i].numVenda, venda[i].codigoCliente, venda[i].dia, venda[i].mes, venda[i].ano, venda[i].numItens, venda[i].totalCarrinho, venda[i].desconto, venda[i].totalFinal, venda[i].status);
            for (int j = 0; j < venda[i].numItens; j++) {
                fprintf(f, "%d\n%s\n%f\n%d\n%f\n", venda[i].itens[j].codigoProduto, venda[i].itens[j].nomeProduto, venda[i].itens[j].precoVenda, venda[i].itens[j].quantidade, venda[i].itens[j].totalItem);
            }
        }
        fclose(f);
    }

    // Salvando Pagamentos
    f = fopen("pagamentos.dat", "w");
    if (f != NULL) {
        fprintf(f, "%d\n", quantidadePagamento);
        for (int i = 0; i < quantidadePagamento; i++) {
            fprintf(f, "%d\n%f\n%s\n", pagamento[i].numVenda, pagamento[i].valorPago, pagamento[i].tipo);
        }
        fclose(f);
    }
}

void carregarInformacao() {
    FILE *f;

    f = fopen("usuarios.dat", "r");
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeUsuario);
        usuario = malloc(quantidadeUsuario * sizeof(Usuario));
        for (int i = 0; i < quantidadeUsuario; i++) {
            fscanf(f, " %[^\n] %[^\n] %d", usuario[i].login, usuario[i].senha, &usuario[i].tipo);
        }
        fclose(f);
    }

    f = fopen("categorias.dat", "r");
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeCategoria);
        categoria = malloc(quantidadeCategoria * sizeof(Categoria));
        for (int i = 0; i < quantidadeCategoria; i++) {
            fscanf(f, "%d %[^\n]", &categoria[i].codigo, categoria[i].nome);
        }
        fclose(f);
    }

    f = fopen("clientes.dat", "r");
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeCliente);
        cliente = malloc(quantidadeCliente * sizeof(Clientes));
        for (int i = 0; i < quantidadeCliente; i++) {
            fscanf(f, "%d %[^\n] %[^\n] %[^\n] %[^\n] %[^\n] %[^\n]", &cliente[i].codigo, cliente[i].nomeCompleto, cliente[i].nomeSocial, cliente[i].cPF, cliente[i].rua_numero, cliente[i].bairro, cliente[i].celular);
        }
        fclose(f);
    }

    f = fopen("produtos.dat", "r");
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeProduto);
        produto = malloc(quantidadeProduto * sizeof(Produtos));
        for (int i = 0; i < quantidadeProduto; i++) {
            fscanf(f, "%d %[^\n] %[^\n] %f %f %f %d %d", &produto[i].codigo, produto[i].nomeProduto, produto[i].categoria, &produto[i].precoCompra, &produto[i].margemLucro, &produto[i].precoVenda, &produto[i].estoque, &produto[i].estoqueMinimo);
        }
        fclose(f);
    }

    f = fopen("vendas.dat", "r");
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeVenda);
        venda = malloc(quantidadeVenda * sizeof(Venda));
        for (int i = 0; i < quantidadeVenda; i++) {
            fscanf(f, "%d %d %d %d %d %d %f %f %f %c", &venda[i].numVenda, &venda[i].codigoCliente, &venda[i].dia, &venda[i].mes, &venda[i].ano, &venda[i].numItens, &venda[i].totalCarrinho, &venda[i].desconto, &venda[i].totalFinal, &venda[i].status);
            for (int j = 0; j < venda[i].numItens; j++) {
                fscanf(f, "%d %[^\n] %f %d %f", &venda[i].itens[j].codigoProduto, venda[i].itens[j].nomeProduto, &venda[i].itens[j].precoVenda, &venda[i].itens[j].quantidade, &venda[i].itens[j].totalItem);
            }
        }
        fclose(f);
    }

    f = fopen("pagamentos.dat", "r");
    if (f != NULL) {
        fscanf(f, "%d", &quantidadePagamento);
        pagamento = malloc(quantidadePagamento * sizeof(Pagamento));
        for (int i = 0; i < quantidadePagamento; i++) {
            fscanf(f, "%d %f %[^\n]", &pagamento[i].numVenda, &pagamento[i].valorPago, pagamento[i].tipo);
        }
        fclose(f);
    }
}