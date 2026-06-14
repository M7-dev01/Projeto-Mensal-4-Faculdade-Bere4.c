#include <stdio.h>
#include <stdlib.h>
#include <string.h> // para manipulação de strings (strcmp, strcpy, etc.)

/* STRUCTS */

// Usuario
/* Armazena os dados de autenticacao dos operadores do sistema.
   Garante restricao de acesso a funcoes administrativas (Caixa/Usuarios).
   O login e a senha */
typedef struct Usuario {
    char login[11];  // Login do usuario
    char senha[9];    // senha
    int tipo;         // 1 = Administrador, 2 = Usuário Comum
} Usuario;

typedef struct Categoria {
    int codigo; // Codigo identificador automatico e unico da categoria
    char nome[50]; // Nome da categoria
} Categoria;


// Clientes
/* Armazena os dados de cada cliente cadastrado.
   O codigo e gerado automaticamente (sequencial).
   Os demais campos sao informados pelo usuario. */
typedef struct Clientes {
    int codigo; // codigo unico do cliente
    char nomeCompleto[80]; // nome completo do cliente
    char nomeSocial[80]; // nome social
    char cPF[15]; // CPF do cliente
    char rua_numero[100]; // endereco
    char bairro[50]; // bairro do cliente
    char celular[20]; // celular
} Clientes;

//Produtos
/* Armazena os dados de cada produto cadastrado.
   O precoVenda e calculado automaticamente com base
   no precoCompra e na margemLucro informados. */
typedef struct Produtos {
    int codigo; // codigo unico do produto
    char nomeProduto[80]; // nome do produto
    char categoria[50]; //categoria 
    float precoCompra; // preco pago na compra do produto
    float margemLucro; // percentual de lucro desejado
    float precoVenda; // preco de venda (calculado: precoCompra + margem)
    int estoque; // quantidade atual em estoque
    int estoqueMinimo; // quantidade minima antes de emitir alerta
} Produtos;

// Item Venda
/* Representa um item (produto) dentro do carrinho de compras.
   Cada venda pode ter ate 50 itens (vetor na struct Venda). */
typedef struct ItemVenda {
    int codigoProduto; // codigo do produto selecionado
    char nomeProduto[80]; // nome do produto
    float precoVenda; // preco no momento da venda
    int quantidade; // quantidade comprada
    float totalItem; // total do item (precoVenda * quantidade)
} ItemVenda;

// Venda
/* Armazena todos os dados de uma venda completa.
   Contem um vetor de ItemVenda para guardar os produtos
   comprados nessa venda. O status indica se a venda
   foi paga ('p') ou esta em aberto ('a'). */
typedef struct Venda {
    int numVenda; // numero identificador da venda
    int codigoCliente; // Vincula cliente na venda (0 para Sem Cliente)
    int dia, mes, ano; // Armazenamento da data da venda para os relatórios por período
    ItemVenda itens[50]; /* Cada venda armazena seus proprios produtos comprados.Cada posicao do vetor representa um item da venda. */
    int numItens; // quantidade de itens adicionados no carrinho
    float totalCarrinho; // soma de todos os itens sem desconto
    float desconto; // percentual de desconto aplicado
    float totalFinal; // valor final apos desconto
    char status; // 'a' = aberto, 'p' = pago
} Venda;

// Pagamento
typedef struct Pagamento {
    int numVenda; // numero da venda que foi paga
    float valorPago; // valor total pago
    char tipo[3]; // 1 = dinheiro, 2 = cartao, 3 = dinheiro+cartao
} Pagamento;


//variavel global
Usuario *usuario = NULL;
/* Cria um ponteiro chamado usuario do tipo Usuario.
   Ele vai armazenar o endereco de memoria dos operadores salvos.
   Comeca com NULL porque a memoria sera alocada dinamicamente. */

int quantidadeUsuario = 0; // total de usuarios cadastrados no sistema
Usuario usuarioLogado; // guarda os dados do usuario que está mexendo no sistema agora
int temUsuarioLogado = 0; // 0 = Ninguém logado (sistema fechado) | 1 = Usuário logado (sistema liberado)

Categoria *categoria = NULL;
/* Cria um ponteiro chamado categoria do tipo Categoria.
   Ele sera usado como vetor dinamico para guardar as categorias.
   Comeca vazio apontando para NULL. */

int quantidadeCategoria = 0; // total de categorias cadastradas no sistema


Clientes *cliente = NULL;
/* Cria um ponteiro chamado cliente do tipo Clientes.
   Ele vai armazenar o endereco de memoria dos clientes cadastrados.
   Comeca com NULL porque ainda nao existe nenhum cliente salvo. */

int quantidadeCliente = 0; // total de clientes cadastrados

Produtos *produto = NULL;
/* Cria um ponteiro chamado produto do tipo Produtos.
   Ele sera usado como vetor dinamico para armazenar os produtos.
   Comeca vazio apontando para NULL. */

int quantidadeProduto = 0; // total de produtos cadastrados

Venda *venda = NULL;
/* Cria um ponteiro chamado venda do tipo Venda.
   Ele vai guardar o endereco das vendas cadastradas dinamicamente.
   No inicio aponta para NULL porque nao existe nenhuma venda. */

int quantidadeVenda = 0; // total de vendas realizadas

Pagamento *pagamento = NULL;
/* Cria um ponteiro chamado pagamento do tipo Pagamento.
   Ele sera usado para armazenar os pagamentos dinamicamente.
   Comeca vazio apontando para NULL. */

int quantidadePagamento = 0; // total de pagamentos registrados

float valorAberturaCaixa = 0; // valor informado na abertura do caixa
float totalretirada = 0; // acumula o total de retirada/sangrias do dia
int caixaAberto = 0; // 0 = fechado, 1 = aberto


/* PROTÓTIPOS DAS FUNÇÕES */
void telaLogin();
int validarAdmin();
void cadastroUsuario();
void cadastroCategoria();
void menuPrincipal();
void menuCadastro();
void menuVendas();
void menuRelatorios();

// Submenus de Relatórios 
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

// Funções de Relatórios e Ordenação 
void relClientesOrdenados();
void relClientesPorPeriodo();
void relProdutosOrdenados();
void relProdutosEstoque();
void relProdutosMaisVendidos();
void relVendasPorPeriodo();
void relFaturamentoConsolidado();


int buscarProduto(int codigo);
int buscarCliente(int codigo);
void exibirProduto();
int dentroDoPeriodo(int d, int m, int a, int dI, int mI, int aI, int dF, int mF, int aF);
void salvarInformacao();
void carregarInformacao();


// Main
int main() {
    carregarInformacao(); // tenta carregar dados de arquivos anteriores
    telaLogin(); // Bloqueio inicial do sistema por autenticação
    menuPrincipal(); // exibe o menu

    // libera a memoria alocada dinamicamente antes de encerrar
    free(usuario);
    free(categoria);
    free(cliente);
    free(produto);
    free(venda);
    free(pagamento);

    return 0;
}


// Tela de Login
/* Exige as credenciais do operador antes de liberar o menu.
   Cria um administrador padrao (adminbere / senha12) caso nao haja dados.
   Permite ate 3 tentativas antes de bloquear totalmente a execucao. */
void telaLogin() {
    char logDigitado[50], senDigitada[50]; // Guarda o login e a senha que a pessoa digitar na tela
    int logado = 0, tentativas = 0; // logado: 0 significa "não entrou" | tentativas: conta quantas vezes errou

    // Se o programa rodar pela primeira vez e não tiver nenhum usuário criado...
    if (quantidadeUsuario == 0) {
        usuario = malloc(sizeof(Usuario)); // Separa um espaço na memória para criar o primeiro usuário

        if (usuario == NULL) {
            printf("Erro de alocacao de memoria!\n");
            exit(1); // Fecha o programa se faltar memória logo de cara
        }

        strcpy(usuario[0].login, "admin1"); // Define que o login padrão
        strcpy(usuario[0].senha, "senha1"); // Define que a senha padrão
        usuario[0].tipo = 1; // Define que ele é Administrador (tipo 1)
        quantidadeUsuario = 1; // Avisa o sistema que agora temos 1 usuário cadastrado
    }

    printf("\n=============================================\n");
    printf("         SISTEMA DA BERE - LOGIN             \n");
    printf("=============================================\n");

    // Fica em loop (repetindo) enquanto a pessoa não logar E não errar mais de 3 vezes
    while (logado == 0 && tentativas < 3) {
        printf("Login: ");
        scanf(" %[^\n]", logDigitado);
        printf("Senha: ");
        scanf(" %[^\n]", senDigitada);

        // Percorre a lista de usuários salvos para ver se o que foi digitado existe
        for (int i = 0; i < quantidadeUsuario; i++) {
            // Se o login E a senha digitados forem iguais aos que estão salvos no sistema
            if (strcmp(usuario[i].login, logDigitado) == 0 && strcmp(usuario[i].senha, senDigitada) == 0) {
                usuarioLogado = usuario[i]; // Salva os dados de quem entrou na variável "usuarioLogado"
                temUsuarioLogado = 1; // Muda para 1 para avisar que o sistema agora está liberado
                logado = 1; // Muda para 1 para quebrar o loop do "while" e dar sucesso
                break;
            }
        }
        // Se depois de passar pelo "for" o sistema ver que a pessoa NÃO conseguiu logar
        if (logado == 0) {
            tentativas++; // Soma +1 no contador de tentativas
            printf("Credenciais incorretas! Tentativa %d de 3.\n\n", tentativas);
        }
    }

    // Se a pessoa errou as 3 vezes e saiu do loop sem conseguir logar
    if (logado == 0) {
        printf("Acesso bloqueado por excesso de tentativas.\n");
        exit(0);
    }
}


// Validacao de Admin
/* Funcao de seguranca interna que checa o nivel de acesso do usuario logado.
   Se for usuario comum, intercepta a operacao e solicita a senha de um Admin
   para autorizar a acao restrita (Ex: Abertura de caixa, sangria, usuarios). */
int validarAdmin() {
    // Se quem já está logado for Administrador (tipo 1), libera
    if (usuarioLogado.tipo == 1){
        return 1;
    }

    char logAdm[50], senAdm[50]; // Cria variáveis para ler um login e senha de gerente/admin
    printf("\n[ACESSO RESTRITO] Informe credenciais de um Administrador:\n");
    printf("Login Admin: ");
    scanf(" %[^\n]", logAdm); // Pede o login do administrador
    printf("Senha Admin: ");
    scanf(" %[^\n]", senAdm); // Pede a senha do administrador

    // Procura na lista de usuários para ver se a senha de admin digitada está certa
    for (int i = 0; i < quantidadeUsuario; i++) {
        // Se o usuário da lista for Admin (tipo 1) E o login e senha digitados baterem
        if (usuario[i].tipo == 1 && strcmp(usuario[i].login, logAdm) == 0 && strcmp(usuario[i].senha, senAdm) == 0) {
            return 1; // Retorna 1 (Libera a função para o funcionário comum)
        }
    }
    // Se o programa passar pelo "for" e não achar nenhum admin válido
    printf("Acesso negado. Operação cancelada.\n");
    return 0; // Retorna 0 (significa "Acesso Bloqueado/Negado")
}


// Menu Principal
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
            case 1:
                menuCadastro(); // abre submenu de cadastros
                break;
            case 2: 
                menuVendas(); // abre submenu de vendas
                break;
            case 3: 
                abrirCaixa(); // abertura do caixa
                break;
            case 4: 
                fecharCaixa(); // fechamento do caixa
                break;
            case 5: 
                menuRelatorios(); // abre submenu de relatorios
                break;
            case 6: 
                salvarInformacao(); // salva as info
                printf("Dados salvos. Saindo...\n"); 
                break;
            default:
                 printf("Opcao invalida!\n");
                 break;
        }
    } while (op != 6);
}

// Menu de Cadastro
/* Submenu para cadastro de usuarios, clientes, produtos e categoria.
   Fica em loop ate o usuario escolher voltar */
void menuCadastro() {
    int op;
    do {
        printf("\n--- MENU CADASTROS ---\n");
        printf("1 - Cadastro de Usuarios\n");   
        printf("2 - Cadastro de Clientes\n");
        printf("3 - Cadastro de Produtos\n");
        printf("4 - Cadastro de Categorias\n"); 
        printf("5 - Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: 
                cadastroUsuario(); // cadastra um no usuario
                break;
            case 2: 
                cadastroCliente(); // cadastra um novo cliente
                break;
            case 3: 
                cadastroProduto(); // cadastra um novo produto
                break;
            case 4: 
                cadastroCategoria(); // cadastra uma nova categoria
                break;
        }
    } while (op != 5);
}

// Menu de VEndas
/* Submenu para operacoes de venda.
   Permite nova venda, sangria e pagamento de
   vendas em aberto. Fica em loop ate voltar */
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
            case 1: 
                novaVenda(); // inicia uma nova venda
                break;
            case 2: 
                retiradaCaixa(); // faz retirada/sangria do caixa
                break;
            case 3: 
                pagarVendaAberta();  // paga venda em aberto
                break;
        }
    } while (op != 4);
}

// Menu de Relatorios
/* Submenu para exibir listagens do sistema.
   Fica em loop ate o usuario escolher voltar */
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
            case 1: 
                menuRelatorioClientes(); // lista todos os clientes
                break;
            case 2: 
                menuRelatorioProdutos(); // lista todos os produtos
                break;
            case 3: 
                menuRelatorioVendas();  // lista todas as vendas
                break;
        }
    } while (op != 4);
}

// Menu de Relatorios de Clientes
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
            case 1: 
                relClientesOrdenados(); 
                break;
            case 2: 
                relClientesPorPeriodo(); 
                break;
        }
    } while (op != 3);
}

// Menu de Relatorios de Produtos
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
            case 1: 
                relProdutosOrdenados(); 
                break;
            case 2: 
                relProdutosEstoque(); 
                break;
            case 3: 
                relProdutosMaisVendidos(); 
                break;
        }
    } while (op != 4);
}

// Menu de Relatorios de Vendas
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
            case 1: 
                relVendasPorPeriodo(); 
                break;
            case 2: 
                relFaturamentoConsolidado(); 
                break;
        }
    } while (op != 3);
}


// Cadastro de Usuario
/* Registra um novo funcionário ou administrador na memória do sistema. */
void cadastroUsuario() {
    if (validarAdmin() == 0) return; // Validação de segurança: se quem estiver mexendo não for admin, cancela na hora

    char tempLog[50], tempSen[50]; // variáveis temporárias para digitar o login e a senha
    int tempTipo; // variável temporária para escolher o nível de acesso

    printf("\n--- CADASTRO DE NOVO USUARIO ---\n");
    // força o usuário a digitar um login válido
    do {
        printf("Login (5 a 10 caracteres): ");
        scanf(" %[^\n]", tempLog);
        if (strlen(tempLog) < 5 || strlen(tempLog) > 10) printf("[ERRO] Tamanho invalido!\n"); // Se for curto ou longo, avisa o erro
    } while (strlen(tempLog) < 5 || strlen(tempLog) > 10); // Repete se o tamanho estiver errado

    // força o usuário a digitar uma senha válida
    do {
        printf("Senha (4 a 8 caracteres): ");
        scanf(" %[^\n]", tempSen);
        if (strlen(tempSen) < 4 || strlen(tempSen) > 8) printf("[ERRO] Tamanho invalido!\n"); // Se for curta ou longa, avisa o erro
    } while (strlen(tempSen) < 4 || strlen(tempSen) > 8); // Repete se o tamanho estiver errado

    // força a escolher o cargo certo
    do {
        printf("Nivel de Acesso (1 - Admin, 2 - Usuario): ");
        scanf("%d", &tempTipo);
    } while (tempTipo != 1 && tempTipo != 2); // Repete se digitarem qualquer número diferente de 1 ou 2

    // Alocação Dinâmica: Se for o primeiro usuário, usa malloc. Se já tiver outros, aumenta o espaço com realloc
    if (quantidadeUsuario == 0) usuario = malloc(sizeof(Usuario));
    else usuario = realloc(usuario, (quantidadeUsuario + 1) * sizeof(Usuario));

    if (usuario == NULL) { // verifica se a alocacao falhou
        printf("Erro de alocacao de memoria!\n");
        return; // Cancela a função para o programa não quebrar
    }

    strcpy(usuario[quantidadeUsuario].login, tempLog); // Copia o login temporário para dentro da lista oficial
    strcpy(usuario[quantidadeUsuario].senha, tempSen); // Copia a senha temporária para dentro da lista oficial
    usuario[quantidadeUsuario].tipo = tempTipo; // Salva o tipo de acesso na lista oficial
    quantidadeUsuario++; // Aumenta +1 o total de usuários cadastrados no sistema
    printf("Usuario cadastrado com sucesso!\n");
}

// Cadastro de Categoria
void cadastroCategoria() {
    printf("\n--- CADASTRO DE CATEGORIA ---\n");
    // Alocação Dinâmica: Se for a primeira categoria usa malloc. Se já tiver outras, usa realloc para abrir espaço
    if (quantidadeCategoria == 0) categoria = malloc(sizeof(Categoria));
    else categoria = realloc(categoria, (quantidadeCategoria + 1) * sizeof(Categoria));

    if (categoria == NULL) { // verifica se a alocacao falhou
        printf("Erro de alocacao de memoria!\n");
        return; // Cancela a função para o programa não quebrar
    }

    categoria[quantidadeCategoria].codigo = quantidadeCategoria + 1; // Gera o código automático sequencial (1, 2, 3...)
    printf("Nome da Categoria: ");
    scanf(" %[^\n]", categoria[quantidadeCategoria].nome); // Lê e salva o nome que o usuário deu para a categoria

    quantidadeCategoria++; // Aumenta +1 o total de categorias cadastradas no sistema
    printf("Categoria cadastrada com sucesso!\n");
}


// Cadastro de Cliente
/* Cadastra um novo cliente no sistema.
   Usa malloc na primeira vez e realloc nas demais
   para expandir o vetor dinamicamente. */
void cadastroCliente() {
    /* Alocacao dinamica: na primeira vez usa malloc para criar espaço na memoria.
    Nas proximas vezes usa realloc para aumentar o vetor dinamico. */
    if (quantidadeCliente == 0) cliente = malloc(sizeof(Clientes));
    else cliente = realloc(cliente, (quantidadeCliente + 1) * sizeof(Clientes));

    if(cliente == NULL){ // verifica se a alocacao falhou
        printf("Erro de alocacao de memoria!\n");
        return;
    }

    cliente[quantidadeCliente].codigo = quantidadeCliente + 1;
    printf("\nNome Completo: "); scanf(" %[^\n]", cliente[quantidadeCliente].nomeCompleto);
    printf("Nome Social: ");   scanf(" %[^\n]", cliente[quantidadeCliente].nomeSocial);
    printf("CPF: ");           scanf(" %[^\n]", cliente[quantidadeCliente].cPF);
    printf("Rua e Numero: ");  scanf(" %[^\n]", cliente[quantidadeCliente].rua_numero);
    printf("Bairro: ");        scanf(" %[^\n]", cliente[quantidadeCliente].bairro);
    printf("Celular: ");       scanf(" %[^\n]", cliente[quantidadeCliente].celular);

    quantidadeCliente++; // incrementa o contador de clientes
    printf("Cliente cadastrado com sucesso! Codigo: %d\n", quantidadeCliente);
}

// CADASTRO DOS PRODUTOS
void cadastroProduto() {
    /* Alocacao dinamica: na primeira vez usa malloc para criar espaço na memoria.
    Nas proximas vezes usa realloc para aumentar o vetor dinamico. */
    if (quantidadeProduto == 0) produto = malloc(sizeof(Produtos));
    else produto = realloc(produto, (quantidadeProduto + 1) * sizeof(Produtos));

    if(produto == NULL){ // verifica se a alocacao falhou
        printf("Erro de alocacao de memoria!\n");
        return;
    }


    produto[quantidadeProduto].codigo = quantidadeProduto + 1000; // Gera o código do produto começando em 1000
    printf("\nNome do Produto: ");
    scanf(" %[^\n]", produto[quantidadeProduto].nomeProduto); // Lê e salva o nome do produto

    // Se já existir alguma categoria cadastrada no sistema
    if (quantidadeCategoria > 0) {
        int sel; // Cria uma variável para o usuário escolher o número da categoria
        printf("Selecione uma Categoria cadastrada:\n");
        // Loop que mostra na tela todas as categorias que existem para o usuário escolher
        for (int i = 0; i < quantidadeCategoria; i++) {
            printf("%d - %s\n", categoria[i].codigo, categoria[i].nome); // Mostra o número e o nome da categoria
        }
        printf("Opcao: ");
        scanf("%d", &sel); // Lê a categoria escolhida pelo usuário

        // Se o número digitado for uma opção válida da lista
        if (sel >= 1 && sel <= quantidadeCategoria) {
            strcpy(produto[quantidadeProduto].categoria, categoria[sel-1].nome); // Copia o nome da categoria escolhida para o produto
        } else {
            strcpy(produto[quantidadeProduto].categoria, "Geral"); // Se digitou errado, joga o produto na categoria "Geral"
        }
    } else {
        // Se não tiver nenhuma categoria criada ainda, deixa o usuário digitar qualquer texto livremente
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


// Nova Venda
/* Realiza uma nova venda com carrinho de compras.
   O usuario adiciona produtos pelo codigo, informa
   a quantidade e pode continuar adicionando itens.
   Ao finalizar, exibe o resumo, aplica desconto
   e encaminha para o processamento do pagamento. */
void novaVenda() {
    // Não deixa vender com caixa fechado
    if (caixaAberto == 0) {
        printf("[ERRO] Nao e possivel realizar vendas sem abrir o caixa!\n");
        return;
    }

    // Bloqueia venda se não houver categorias cadastradas
    if (quantidadeCategoria == 0) {
        printf("[ALERTA] Nao e possivel realizar vendas! Nenhuma CATEGORIA foi cadastrada no sistema ainda.\n");
        printf("Por favor, cadastre pelo menos uma categoria no Menu de Cadastros antes de vender.\n");
        return;
    }

    // Bloqueia venda se não houver produtos cadastrados
    if (quantidadeProduto == 0) {
        printf("[ALERTA] Nao e possivel realizar vendas! Nenhum PRODUTO foi cadastrada no sistema ainda.\n");
        printf("Por favor, cadastre pelo menos um produto no Menu de Cadastros antes de vender.\n");
        return;
    }

    /* Alocacao dinamica: na primeira vez usa malloc para criar espaço na memoria.
    Nas proximas vezes usa realloc para aumentar o vetor dinamico. */
    if (quantidadeVenda == 0) venda = malloc(sizeof(Venda));
    else venda = realloc(venda, (quantidadeVenda + 1) * sizeof(Venda));

     if(venda == NULL){ // verifica se a alocacao falhou
        printf("Erro de alocacao de memoria!\n");
        return;
    }

    // inicializa os dados da nova venda
    venda[quantidadeVenda].numVenda = quantidadeVenda + 1; // numero sequencial
    venda[quantidadeVenda].numItens = 0; // começa sem itens
    venda[quantidadeVenda].totalCarrinho = 0; // total zerado
    venda[quantidadeVenda].status = 'a'; // começa em aberto


    // Solicitação e vinculação obrigatória de Cliente e Data da Compra
    printf("\nCodigo do Cliente (0 para Consumidor Comum): ");
    scanf("%d", &venda[quantidadeVenda].codigoCliente); // Lê o código do cliente que está comprando
    printf("Informe a Data da Venda (DD MM AAAA): ");
    scanf("%d %d %d", &venda[quantidadeVenda].dia, &venda[quantidadeVenda].mes, &venda[quantidadeVenda].ano); // Lê o dia, mês e ano da venda

    char op = 's'; // variável de controle para o loop de adicionar produtos
    while (op == 's' || op == 'S') { // Fica repetindo enquanto o usuário digitar 's' ou 'S'
        exibirProduto(); // mostra a tabela de produtos disponiveis
        int codProd, qtd; // variáveis para o código do produto e a quantidade que o cliente quer
        printf("\nInforme o Codigo do Produto para Adicionar ao Carrinho: ");
        scanf("%d", &codProd);
        int idx = buscarProduto(codProd); // Procura a posição real desse produto no vetor dinâmico

        if (idx != -1) { // Se achou o produto (retorno diferente de -1)
            if (produto[idx].estoque == 0) { // Verifica se o estoque desse item está zerado
                printf("[ALERTA] Estoque zerado! Operacao cancelada para este item.\n");
                continue; // Pula o resto do bloco e volta pro começo do loop do carrinho
            }
            printf("Quantidade desejada: ");
            scanf("%d", &qtd); // Lê quantas unidades o cliente quer levar

            if (qtd > produto[idx].estoque) { // Se o cliente pedir mais do que tem no estoque
                printf("Estoque insuficiente. Deseja levar o saldo total de %d unidades? (s/n): ", produto[idx].estoque);
                char conf; scanf(" %c", &conf); // Lê a confirmação do usuário
                if (conf == 's' || conf == 'S') qtd = produto[idx].estoque; // Se aceitou, leva tudo o que sobrou no estoque
                else continue; // Se não aceitou, ignora o item e volta pro início do loop
            }

            int p = venda[quantidadeVenda].numItens; // Pega a posição atual livre dentro do carrinho dessa venda
            venda[quantidadeVenda].itens[p].codigoProduto = produto[idx].codigo; // Guarda o código do produto no carrinho
            strcpy(venda[quantidadeVenda].itens[p].nomeProduto, produto[idx].nomeProduto); // Copia o nome do produto pro carrinho
            venda[quantidadeVenda].itens[p].precoVenda = produto[idx].precoVenda; // Salva o preço de venda dele na estrutura da venda
            venda[quantidadeVenda].itens[p].quantidade = qtd; // Salva a quantidade que está sendo levada
            venda[quantidadeVenda].itens[p].totalItem = produto[idx].precoVenda * qtd; // Calcula o preço total desse item (Preço x Qtd)

            venda[quantidadeVenda].totalCarrinho += venda[quantidadeVenda].itens[p].totalItem; // Soma o total do item ao valor acumulado do carrinho
            venda[quantidadeVenda].numItens++; // Aumenta em +1 a quantidade de tipos de itens dentro do carrinho
            produto[idx].estoque -= qtd; // Abaixa a quantidade do estoque real do produto

            if (produto[idx].estoque <= produto[idx].estoqueMinimo) { // Verifica se o estoque caiu abaixo do limite de segurança
                printf("[ALERTA] O produto %s atingiu o nivel de estoque minimo!\n", produto[idx].nomeProduto); // Alerta
            }
        } else { // Se a função buscarProduto retornar -1
            printf("Produto nao cadastrado!\n"); // Avisa que o código digitado não existe
        }
        printf("Deseja inserir mais produtos? (s/n): ");
        scanf(" %c", &op); // Pergunta se quer continuar comprando
    }

    printf("Porcentagem de Desconto (0 para nenhum): ");
    scanf("%f", &venda[quantidadeVenda].desconto);
    // Faz a conta matemática: Valor do Carrinho - (Valor do Carrinho * Porcentagem / 100)
    venda[quantidadeVenda].totalFinal = venda[quantidadeVenda].totalCarrinho - (venda[quantidadeVenda].totalCarrinho * venda[quantidadeVenda].desconto / 100);

    quantidadeVenda++; // Finaliza a montagem da venda e incrementa o contador global de vendas
    processarPagamento(quantidadeVenda - 1); // Envia essa venda recém-criada para a função de pagamento
}

void processarPagamento(int idx) {
    printf("\nTotal a Pagar: R$ %.2f\n", venda[idx].totalFinal); // Mostra o valor final líquido da venda
    printf("Formas de Pagamento:\n1 - Dinheiro\n2 - Cartao\n3 - Misto (Dinheiro + Cartao)\nOpcao: ");
    int forma; 
    scanf("%d", &forma);// Lê a forma de pagamento escolhida

    if (quantidadePagamento == 0) pagamento = malloc(sizeof(Pagamento)); // Aloca memória inicial se for o primeiro pagamento do sistema
    
    if (forma == 1) { // Pagamento em dinehiro
        float valorEntregue; // Variável para ler quanto em cédulas o cliente deu
        do {
            printf("Valor entregue pelo cliente: R$ ");
            scanf("%f", &valorEntregue); // Lê o dinheiro entregue
            if (valorEntregue < venda[idx].totalFinal) { // Validação defensiva: se der menos dinheiro do que a conta
                printf("[ERRO] Valor insuficiente! O total e R$ %.2f. Digite novamente.\n", venda[idx].totalFinal);
            }
        } while (valorEntregue < venda[idx].totalFinal); // Obriga a repetir até dar dinheiro suficiente

        float troco = valorEntregue - venda[idx].totalFinal; // Calcula o troco (Dinheiro Entregue - Total da Conta)
        if (troco > 0) {
            printf("TROCO: R$ %.2f\n", troco); // Mostra o valor do troco
        } else {
            printf("Sem troco (valor exato).\n"); // Se for igual, avisa que não tem troco
        }

        // Realoca espaço na memória dinâmica para salvar esse novo registro de pagamento
        pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento));
        pagamento[quantidadePagamento].numVenda = venda[idx].numVenda; // Vincula o pagamento ao número da venda
        pagamento[quantidadePagamento].valorPago = venda[idx].totalFinal; // Grava o valor quitado
        strcpy(pagamento[quantidadePagamento].tipo, "d"); // Salva o tipo como "d" (Dinheiro)
        quantidadePagamento++; // Aumenta o contador de registros de pagamentos
        venda[idx].status = 'p'; // Atualiza o status da venda para 'p' pago
        printf("Pagamento em dinheiro registrado com sucesso!\n"); 
    } 
    else if (forma == 2) { //Pagamento no cartão
        int statusMaquina;
        printf("\n[MAQUININHA] Processando... Insira, passe ou aproxime o cartao.\n");
        printf("Simular retorno da máquina (1 - Aprovado / 2 - Recusado/Erro): ");
        scanf("%d", &statusMaquina);

        if (statusMaquina == 1) { // Se for aprovada 1
            pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento)); // Abre espaço na memória
            pagamento[quantidadePagamento].numVenda = venda[idx].numVenda; // Vincula o número da venda
            pagamento[quantidadePagamento].valorPago = venda[idx].totalFinal; // Salva o valor pago
            strcpy(pagamento[quantidadePagamento].tipo, "c"); // Salva o tipo como "c" cartão
            quantidadePagamento++; // Sobe o contador de pagamentos
            venda[idx].status = 'p'; // Quita a venda mudando o status para 'p'
            printf("[SUCESSO] Transacao autorizada pela operadora do cartao!\n");
        } else {
            // Se for recusado 2
            printf("[ALERTA] Transacao Recusada ou Cancelada. A venda ficara EM ABERTO para posterior liquidacao.\n");
            venda[idx].status = 'a'; // Mantém a venda com status 'a' (Em Aberto) para poder cobrar depois
        }
    } 
    else if (forma == 3) { // Pagamento misto cartão + dinheiro
        float valorDinheiro, valorCartao; // Guarda as divisões do valor total
        float valorEntregueDinheiro; // Guarda quanto dinheiro físico foi entregue

        do {
            printf("Valor que sera pago em Dinheiro: R$ ");
            scanf("%f", &valorDinheiro); // Lê a parte que vai ser paga em dinheiro
            if (valorDinheiro > venda[idx].totalFinal) { // Impede o dinheiro de ser maior do que a própria conta
                printf("[ERRO] O valor em dinheiro nao pode ser maior que o total da venda (R$ %.2f).\n", venda[idx].totalFinal);
            }
        } while (valorDinheiro > venda[idx].totalFinal); // Repete o bloco se o valor digitado for inválido

        do {
            printf("Quantia fisica entregue em dinheiro para pagar os R$ %.2f: R$ ", valorDinheiro);
            scanf("%f", &valorEntregueDinheiro); // Lê o valor em papel moeda entregue
            if (valorEntregueDinheiro < valorDinheiro) { // Valida se o dinheiro cobre a parte combinada do dinheiro
                printf("[ERRO] Quantia insuficiente para cobrir a parte em dinheiro!\n");
            }
        } while (valorEntregueDinheiro < valorDinheiro); // Repete se faltar dinheiro

        float trocoMisto = valorEntregueDinheiro - valorDinheiro; // Calcula o troco apenas da parte paga em dinheiro
        if (trocoMisto > 0) printf("==> TROCO DA PARTE EM DINHEIRO: R$ %.2f\n", trocoMisto);

        valorCartao = venda[idx].totalFinal - valorDinheiro; // Calcula o restante que sobrou para passar no cartão
        printf("Restante a pagar no Cartao: R$ %.2f\n", valorCartao);

        int statusMaquinaMisto; // Variável para simular a maquininha na operação mista
        printf("Status da máquina para a parte do Cartao (1 - Aprovado / 2 - Recusado): ");
        scanf("%d", &statusMaquinaMisto); // Lê o resultado da simulação

        if (statusMaquinaMisto == 1) { // Se a maquininha aprovou a parte do cartão, grava as duas partes do pagamento
            // Grava o pedaço do Dinheiro na memória dinâmica
            pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento));
            pagamento[quantidadePagamento].numVenda = venda[idx].numVenda;
            pagamento[quantidadePagamento].valorPago = valorDinheiro;
            strcpy(pagamento[quantidadePagamento].tipo, "dc"); // Define "dc" Dinheiro vindo de Misto
            quantidadePagamento++;

            // Grava o pedaço do Cartão na memória dinâmica
            pagamento = realloc(pagamento, (quantidadePagamento + 1) * sizeof(Pagamento));
            pagamento[quantidadePagamento].numVenda = venda[idx].numVenda;
            pagamento[quantidadePagamento].valorPago = valorCartao;
            strcpy(pagamento[quantidadePagamento].tipo, "dc"); // Define "dc" também para rastrear no faturamento
            quantidadePagamento++;

            venda[idx].status = 'p'; // Muda o status da venda para 'p' pago com sucesso
            printf("[SUCESSO] Pagamento misto concluido com êxito!\n");
        } else { // Se o cartão falhar, a operação inteira é abortada por segurança
            printf("[ALERTA] Falha na aprovacao do cartao. Operacao misto abortada. A venda ficara EM ABERTO.\n");
            venda[idx].status = 'a'; // Mantém em aberto para tentar de novo no menu adequado
        }
    }
}

void pagarVendaAberta() {
    if (caixaAberto == 0) { // Não deixa mexer se o caixa estiver fechado
        printf("[ERRO] Caixa fechado!\n"); 
        return; 
    } 
    printf("\n--- VENDAS EM ABERTO ---\n");
    int achou = 0; // Variável para controlar se existem vendas em aberto no sistema
    // Loop para varrer todas as vendas cadastradas
    for (int i = 0; i < quantidadeVenda; i++) {
        if (venda[i].status == 'a') { // Se o status for igual a 'a' Em Aberto
            printf("Venda #%d - Total: R$ %.2f\n", venda[i].numVenda, venda[i].totalFinal);
            achou = 1; // Marca que encontrou pelo menos um registro
        }
    }
    if (achou == 0) { // Se passou pelo laço e não achou nada, sai da função
        printf("Nenhuma venda em aberto encontrada.\n"); 
        return; 
    }

    int num;
    printf("\nInforme o numero da venda que deseja liquidar: ");
    scanf("%d", &num); // Lê o número da venda que o usuário quer pagar
    // Loop para achar a venda correspondente ao número digitado
    for (int i = 0; i < quantidadeVenda; i++) { 
        if (venda[i].numVenda == num && venda[i].status == 'a') { // Se o número bater E ela ainda estiver em aberto.
            processarPagamento(i); // Envia o índice dela para a função processarPagamento resolver a cobrança
            return; // Termina a função com sucesso
        }
    }
    printf("Venda invalida ou ja paga.\n"); // Se o número digitado não bater com nenhuma em aberto, avisa o erro
}

void abrirCaixa() {
    if (caixaAberto == 1) { // Evita abrir o caixa duas vezes seguidas
        printf("O caixa ja esta aberto!\n"); 
        return; 
    }
    if (validarAdmin() == 0) return; // Validação de segurança: se quem estiver mexendo não for admin, cancela na hora

    printf("Valor de Abertura do Caixa: R$ "); 
    scanf("%f", &valorAberturaCaixa); // Lê o valor do troco inicial que fica na gaveta do caixa (fundo de caixa)
    totalretirada = 0; // Zera a variável que conta as sangrias feitas no dia
    caixaAberto = 1; // Altera para 1 para sinalizar que o sistema está liberado para realizar vendas
    printf("Caixa aberto com sucesso!\n");
}

void retiradaCaixa() {
    if (caixaAberto == 0) { // Impede sangria com o caixa fechado
        printf("Abra o caixa primeiro!\n"); 
        return; 
    }
    if (validarAdmin() == 0) return; // Validação de segurança: se quem estiver mexendo não for admin, cancela na hora

    float valor; // variavel para ler valor retirado
    printf("Valor da Retirada (Sangria): R$ ");
    scanf("%f", &valor);
    totalretirada += valor; // Soma esse valor no acumulador global de sangrias do dia
    printf("Sangria registrada!\n");
}

void fecharCaixa() {
    if (caixaAberto == 0) { // Impede fechar um caixa que já está fechado
        printf("O caixa ja esta fechado!\n"); 
        return; 
    }
    if (validarAdmin() == 0) return; // Validação de segurança: se quem estiver mexendo não for admin, cancela na hora

    float dinheiroEmCaixa, totalDinheiroVendas = 0, totalCartaoVendas = 0;
    printf("Informe o valor total em dinheiro FISICO presente no caixa: R$ ");
    scanf("%f", &dinheiroEmCaixa);

    // Loop que varre todos os pagamentos para consolidar o que entrou no sistema eletrônico
    for (int i = 0; i < quantidadePagamento; i++) {
        if (strcmp(pagamento[i].tipo, "d") == 0) totalDinheiroVendas += pagamento[i].valorPago; // Se o tipo for "d", soma no dinheiro
        else if (strcmp(pagamento[i].tipo, "c") == 0) totalCartaoVendas += pagamento[i].valorPago; // Se for "c", soma no cartão
        else if (strcmp(pagamento[i].tipo, "dc") == 0) {

            totalDinheiroVendas += pagamento[i].valorPago; // Soma a parte física no montante de dinheiro
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
    printf("Saldo Esperado:   R$ %.2f\n", saldoDinheiroEsperado); // Mostra quanto o sistema calculou que deveria ter no caixa
    printf("Dinheiro Fisico:  R$ %.2f\n", dinheiroEmCaixa); // Mostra quanto o funcionário disse que contou na vida real

    if (dinheiroEmCaixa != saldoDinheiroEsperado) { // Se o valor físico real for diferente do esperado pelo cálculo
        printf("[ALERTA CRITICO] O VALOR ENCONTRADO NAO BATE COM O SALDO ESPERADO!\n");
        printf("Divergencia de: R$ %.2f\n", dinheiroEmCaixa - saldoDinheiroEsperado); // Mostra a diferença (se faltou ou sobrou dinheiro)
    } else {
        printf("Caixa fechado com sucesso sem divergencias de saldo!\n"); // Sucesso total, as contas bateram centavo por centavo
    }

    caixaAberto = 0; //Altera para 0, bloqueando novas operações até a próxima abertura
}


// Listagem Alfabética de Clientes
void relClientesOrdenados() {
    if (quantidadeCliente == 0) { // Se não houver clientes, cancela a listagem
        printf("Nenhum cliente cadastrado.\n"); 
        return; 
    }

    // Aloca um vetor temporário (aux) na memória para clonar a lista de clientes original
    Clientes *aux = malloc(quantidadeCliente * sizeof(Clientes));
    memcpy(aux, cliente, quantidadeCliente * sizeof(Clientes)); // Copia em bloco todos os dados do vetor cliente para o aux

    // Laço Externo (i): Controla o número máximo de passagens completas pelo vetor.
    // A cada rodada completa do laço interno, o maior elemento restante "flutua" para o final.
    for (int i = 0; i < quantidadeCliente - 1; i++) { 
        // Laço Interno (j): Percorre o vetor comparando pares de elementos vizinhos (j e j+1).
        // A condição "quantidadeCliente - i - 1" evita retestar os elementos que já
        // foram empurrados para o final e já estão na posição correta nas rodadas anteriores.
        for (int j = 0; j < quantidadeCliente - i - 1; j++) {
            // strcmp compara duas strings caractere por caractere usando a tabela ASCII.
            // Se retornar > 0, significa que o nome em aux[j] vem DEPOIS do nome em aux[j+1].
            // Ou seja, eles estão fora de ordem alfabética e precisam ser trocados.
            if (strcmp(aux[j].nomeCompleto, aux[j+1].nomeCompleto) > 0) {
                // Processo de Troca: Usa uma variável temporária para inverter as posições
                Clientes temp = aux[j]; // 1. Guarda o cliente atual na gaveta temporária
                aux[j] = aux[j+1]; // 2. Coloca o cliente da frente na posição atual
                aux[j+1] = temp; // 3. Coloca o cliente guardado na gaveta na posição da frente
            }
        }
    }

    printf("\n--- LISTAGEM DE CLIENTES EM ORDEM ALFABETICA ---\n");
    for (int i = 0; i < quantidadeCliente; i++) { // Loop de exibição formatada da lista ordenada
        printf("Cod: %d | Nome: %s | CPF: %s\n", aux[i].codigo, aux[i].nomeCompleto, aux[i].cPF);
    }
    free(aux); // Libera o espaço da memória dinâmica do vetor auxiliar para não dar vazamento de memória
}

// Filtra Clientes por Período de Compra
void relClientesPorPeriodo() {
    int dI, mI, aI, dF, mF, aF; // variáveis para capturar a data de Início (I) e data de Fim (F)
    printf("Data Inicial (DD MM AAAA): "); scanf("%d %d %d", &dI, &mI, &aI); // Lê a data inicial do filtro
    printf("Data Final (DD MM AAAA): ");   scanf("%d %d %d", &dF, &mF, &aF); // Lê a data final do filtro

    printf("\n--- CLIENTES QUE COMPRARAM NO PERIODO ---\n");
    int achou = 0;
    // Loop para percorrer o histórico de todas as vendas registradas
    for (int i = 0; i < quantidadeVenda; i++) {
        // Envia a data da venda atual para a função auxiliar dentroDoPeriodo para validação
        if (dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) {
            int idxCli = buscarCliente(venda[i].codigoCliente); // Localiza em qual índice do vetor está o cliente dessa venda
            if (idxCli != -1) { // Se achou o cliente cadastrado correspondente
                printf("Cliente: %s | Comprou na Venda #%d em %02d/%02d/%04d\n", cliente[idxCli].nomeCompleto, venda[i].numVenda, venda[i].dia, venda[i].mes, venda[i].ano);
                achou = 1;
            }
        }
    }
    if (achou == 0) printf("Nenhum registro de compra encontrado no periodo informado.\n");
}

// Listagem Alfabética de Produtos
void relProdutosOrdenados() {
    if (quantidadeProduto == 0) { // Cancela se o vetor estiver vazio
        printf("Nenhum produto cadastrado.\n"); 
        return; 
    }

    // Aloca memória e clona a lista original de produtos para ordenar no espelho temporário, preservando o código sequencial original
    Produtos *aux = malloc(quantidadeProduto * sizeof(Produtos));
    memcpy(aux, produto, quantidadeProduto * sizeof(Produtos));

    // Bubble Sort para organizar os produtos por nome em ordem alfabética pura (A-Z)
    for (int i = 0; i < quantidadeProduto - 1; i++) {
        for (int j = 0; j < quantidadeProduto - i - 1; j++) {
            if (strcmp(aux[j].nomeProduto, aux[j+1].nomeProduto) > 0) { // Compara os nomes das duas posições vizinhas
                Produtos temp = aux[j]; // 1. Guarda o produto atual na gaveta temporária
                aux[j] = aux[j+1]; // 2. Coloca o produto da frente na posição atual
                aux[j+1] = temp; // 3. Coloca o produto guardado na gaveta na posição da frente
            }
        }
    }

    printf("\n--- LISTAGEM DE PRODUTOS EM ORDEM ALFABETICA ---\n");
    for (int i = 0; i < quantidadeProduto; i++) { // Loop de exibição formatada da lista ordenada
        printf("Cod: %d | Nome: %s | Preco Venda: R$ %.2f | Estoque: %d\n", aux[i].codigo, aux[i].nomeProduto, aux[i].precoVenda, aux[i].estoque);
    }
    free(aux); // Libera o espaço da memória dinâmica do vetor auxiliar para não dar vazamento de memória
}

// Listagem de Estoques Separados
void relProdutosEstoque() {
    printf("\n--- PRODUTOS COM ESTOQUE ZERADO ---\n");
    for (int i = 0; i < quantidadeProduto; i++) { // Percorre a lista de mercadorias procurando itens com estoque zerado
        if (produto[i].estoque == 0) printf("Cod: %d | Nome: %s\n", produto[i].codigo, produto[i].nomeProduto); // Lista se estiver zerado
    }
    printf("\n--- PRODUTOS ABAIXO DO ESTOQUE MINIMO ---\n");
    for (int i = 0; i < quantidadeProduto; i++) { // Percorre procurando itens com estoque minimo
        // Condição: Tem que ter pelo menos 1 unidade no estoque físico E ser menor ou igual ao limite de estoque minimo
        if (produto[i].estoque > 0 && produto[i].estoque <= produto[i].estoqueMinimo) {
            printf("Cod: %d | Nome: %s | Atual: %d | Minimo: %d\n", produto[i].codigo, produto[i].nomeProduto, produto[i].estoque, produto[i].estoqueMinimo);
        }
    }
}

// Ranking dos Produtos Mais Vendidos por Período
void relProdutosMaisVendidos() {
    int dI, mI, aI, dF, mF, aF; // Guarda as datas inicial e final digitadas pelo usuário
    printf("Data Inicial (DD MM AAAA): "); 
    scanf("%d %d %d", &dI, &mI, &aI);
    printf("Data Final (DD MM AAAA): ");   
    scanf("%d %d %d", &dF, &mF, &aF);

    // struct local temporária para montar o ranking
    typedef struct { int cod; char nome[80]; int qtdVendida; } Rank;
    // calloc: Aloca memória para a estrutura e zera automaticamente todos os campos numéricos por segurança
    Rank *ranking = calloc(quantidadeProduto, sizeof(Rank));

    // Copia o código e o nome de todos os produtos cadastrados para o vetor do ranking
    for (int i = 0; i < quantidadeProduto; i++) {
        ranking[i].cod = produto[i].codigo; // copia codigo do produto
        strcpy(ranking[i].nome, produto[i].nomeProduto); // copia nome do produto
    }

    // Passa por todas as vendas registradas no sistema
    for (int i = 0; i < quantidadeVenda; i++) {
        // Verifica se a venda atual aconteceu dentro do período escolhido
        if (dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) {
            for (int j = 0; j < venda[i].numItens; j++) { // Passa pelos itens comprados nessa venda
                for (int k = 0; k < quantidadeProduto; k++) { // Procura o produto correspondente no ranking
                    if (ranking[k].cod == venda[i].itens[j].codigoProduto) {
                        ranking[k].qtdVendida += venda[i].itens[j].quantidade; // Soma a quantidade vendida ao total do produto
                    }
                }
            }
        }
    }

    // Bubble Sort: Ordena os produtos do que vendeu MAIS para o que vendeu MENOS
    for (int i = 0; i < quantidadeProduto - 1; i++) {
        for (int j = 0; j < quantidadeProduto - i - 1; j++) {
            // Se o item atual vendeu menos que o próximo, eles trocam de posição
            if (ranking[j].qtdVendida < ranking[j+1].qtdVendida) {
                Rank temp = ranking[j]; // Troca as posições usando a variável temporária temp
                ranking[j] = ranking[j+1];
                ranking[j+1] = temp;
            }
        }
    }

    printf("\n--- PRODUTOS MAIS VENDIDOS NO PERIODO ---\n");
    for (int i = 0; i < quantidadeProduto; i++) { // Percorre a lista ordenada para mostrar na tela
        if (ranking[i].qtdVendida > 0) { // Só mostra produtos que tiveram pelo menos 1 unidade vendida
            printf("Posicao %d | Nome: %s | Qtd Vendida: %d\n", i+1, ranking[i].nome, ranking[i].qtdVendida);
        }
    }
    free(ranking); // Libera a memória usada pelo vetor temporário do ranking
}

// Listagem de Vendas por Período
void relVendasPorPeriodo() {
    int dI, mI, aI, dF, mF, aF;
    printf("Data Inicial (DD MM AAAA): "); 
    scanf("%d %d %d", &dI, &mI, &aI);
    printf("Data Final (DD MM AAAA): ");   
    scanf("%d %d %d", &dF, &mF, &aF);

    printf("\n--- VENDAS REALIZADAS NO PERIODO ---\n");
    for (int i = 0; i < quantidadeVenda; i++) { // Percorre todas as vendas do sistema
        if (dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) { // Checa a data da venda
            
            char statusTexto[15]; //guardar o texto do status

            if (venda[i].status == 'p') {
                strcpy(statusTexto, "Paga"); // Copia "Paga" para a variável
            } 
            else {
                strcpy(statusTexto, "Em Aberto");  // Copia "Em Aberto" para a variável
            }

            printf("Venda #%d | Data: %02d/%02d/%04d | Total Final: R$ %.2f | Status: %s\n",
                venda[i].numVenda, venda[i].dia, venda[i].mes, venda[i].ano, venda[i].totalFinal, statusTexto);
        }
    }
}

// Faturamento Consolidado (Fechamento de Caixa por Período)
void relFaturamentoConsolidado() {
    int dI, mI, aI, dF, mF, aF;
    printf("Data Inicial (DD MM AAAA): "); 
    scanf("%d %d %d", &dI, &mI, &aI);
    printf("Data Final (DD MM AAAA): ");   
    scanf("%d %d %d", &dF, &mF, &aF);

    float fatDinheiro = 0, fatCartao = 0, fatTotal = 0; // Variáveis para somar os totais de cada tipo de pagamento

    for (int i = 0; i < quantidadeVenda; i++) {
        // A venda só entra no cálculo se o status for pago ('p') E estiver dentro do período digitado
        if (venda[i].status == 'p' && dentroDoPeriodo(venda[i].dia, venda[i].mes, venda[i].ano, dI, mI, aI, dF, mF, aF)) {
            fatTotal += venda[i].totalFinal; // Soma o valor da venda ao faturamento bruto total
            for (int j = 0; j < quantidadePagamento; j++) { // Varre a lista de pagamentos para descobrir a forma de pagamento
                if (pagamento[j].numVenda == venda[i].numVenda) { // Se o pagamento pertencer a esta venda
                    if (strcmp(pagamento[j].tipo, "d") == 0) fatDinheiro += pagamento[j].valorPago; // Se foi dinheiro, soma no dinheiro
                    else if (strcmp(pagamento[j].tipo, "c") == 0) fatCartao += pagamento[j].valorPago; // Se foi cartão, soma no cartão
                    else if (strcmp(pagamento[j].tipo, "dc") == 0) { 
                        fatDinheiro += pagamento[j].valorPago;
                    }
                }
            }
        }
    }

    printf("\n--- FATURAMENTO CONSOLIDADO DO PERIODO ---\n");
    printf("Total Entrado em Dinheiro: R$ %.2f\n", fatDinheiro);
    printf("Total Entrado em Cartao:   R$ %.2f\n", fatCartao);
    printf("FATURAMENTO BRUTO TOTAL:   R$ %.2f\n", fatTotal); // Soma geral de tudo que entrou no caixa
}


// Buscar Produtos por codifo
// percorre o vetor de produtos procurando pelo codigo informado. Retorna o indice do produto se encontrado, ou -1 caso nao exista.
int buscarProduto(int codigo) {
    for (int i = 0; i < quantidadeProduto; i++) { 
        if (produto[i].codigo == codigo){ // encontrou o produto 
        return i; // retorna o indice
        }
    }
    return -1; // produto nao encontrado
}

// Procura um cliente pelo código digitado
int buscarCliente(int codigo) {
    // Busca sequencial no vetor de clientes
    for (int i = 0; i < quantidadeCliente; i++) {
        if (cliente[i].codigo == codigo) return i; // Retorna a posição do cliente no vetor caso encontre
    }
    return -1; // Retorna -1 se o cliente não existir
}

// Exibe todos os produtos cadastrados
void exibirProduto() {
    printf("\n--- PRODUTOS DISPONIVEIS ---\n");
    for (int i = 0; i < quantidadeProduto; i++) { // Passa por todo o vetor imprimindo os dados
        printf("Cod: %d | Produto: %s | Preco: R$ %.2f | Estoque: %d\n", produto[i].codigo, produto[i].nomeProduto, produto[i].precoVenda, produto[i].estoque);
    }
}

// Verifica se uma data de registro está dentro de um período (inicial e final)
int dentroDoPeriodo(int d, int m, int a, int dI, int mI, int aI, int dF, int mF, int aF) {
    // Transforma as datas em números inteiros grandes no formato AAAAMMDD para facilitar a comparação matemática
    long dataReg = a * 10000 + m * 100 + d; // Exemplo: 14/06/2026 vira 20260614
    long dataIni = aI * 10000 + mI * 100 + dI; // Data inicial convertida
    long dataFim = aF * 10000 + mF * 100 + dF; // Data final convertida

    // Retorna 1 (Verdadeiro) se a data estiver entre o início e o fim, ou 0 (Falso) se estiver fora
    return (dataReg >= dataIni && dataReg <= dataFim);
}


// Salvando a informação
/* Salva todas as info em arquivo de texto (.dat) pra que na proxima vez que rodar o programa já tenha as info.*/
void salvarInformacao() {
    FILE *f; // ponteiro para arquivo

    // Salvando Usuários
    f = fopen("usuarios.dat", "w"); // abre o arquivo para escrita
    if (f != NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(f, "%d\n", quantidadeUsuario); // Escreve na primeira linha o total de usuários na memória
        for (int i = 0; i < quantidadeUsuario; i++) { // Passa gravando os dados de cada usuário linha por linha
            fprintf(f, "%s\n%s\n%d\n", usuario[i].login, usuario[i].senha, usuario[i].tipo);
        }
        fclose(f); // Fecha o arquivo para salvar as alterações e liberar a memória do sistema
    }

    // Salvando Categorias
    f = fopen("categorias.dat", "w"); // abre o arquivo para escrita
    if (f != NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(f, "%d\n", quantidadeCategoria); // Salva a quantidade de categorias cadastradas
        for (int i = 0; i < quantidadeCategoria; i++) {
            fprintf(f, "%d\n%s\n", categoria[i].codigo, categoria[i].nome); // Grava o código e o nome da categoria
        }
        fclose(f); // Fecha o arquivo para salvar as alterações e liberar a memória do sistema
    } 

    // Salvando Clientes
    f = fopen("clientes.dat", "w"); // abre o arquivo para escrita
    if (f != NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(f, "%d\n", quantidadeCliente); // Salva o total de clientes na primeira linha
        for (int i = 0; i < quantidadeCliente; i++) {
            // Grava toda a ficha cadastral do cliente que está na struct
            fprintf(f, "%d\n%s\n%s\n%s\n%s\n%s\n%s\n", cliente[i].codigo, cliente[i].nomeCompleto, cliente[i].nomeSocial, cliente[i].cPF, cliente[i].rua_numero, cliente[i].bairro, cliente[i].celular);
        }
        fclose(f); // Fecha o arquivo para salvar as alterações e liberar a memória do sistema
    }

    // Salvando Produtos
    f = fopen("produtos.dat", "w"); // abre o arquivo para escrita
    if (f != NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(f, "%d\n", quantidadeProduto); // Grava o total de produtos ativos no sistema
        for (int i = 0; i < quantidadeProduto; i++) {
            // Grava os dados do produto (Textos, valores de compra/venda e quantidades de estoque)
            fprintf(f, "%d\n%s\n%s\n%f\n%f\n%f\n%d\n%d\n", produto[i].codigo, produto[i].nomeProduto, produto[i].categoria, produto[i].precoCompra, produto[i].margemLucro, produto[i].precoVenda, produto[i].estoque, produto[i].estoqueMinimo);
        }
        fclose(f); // Fecha o arquivo para salvar as alterações e liberar a memória do sistema
    }

    // Salvando Vendas e Itens
    f = fopen("vendas.dat", "w"); // abre o arquivo para escrita
    if (f != NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(f, "%d\n", quantidadeVenda); // Grava a quantidade total de vendas já feitas no mercado
        for (int i = 0; i < quantidadeVenda; i++) { // Primeiro laço: Percorre e salva o cabeçalho de cada venda
            fprintf(f, "%d\n%d\n%d\n%d\n%d\n%d\n%f\n%f\n%f\n%c\n", venda[i].numVenda, venda[i].codigoCliente, venda[i].dia, venda[i].mes, venda[i].ano, venda[i].numItens, venda[i].totalCarrinho, venda[i].desconto, venda[i].totalFinal, venda[i].status);
            for (int j = 0; j < venda[i].numItens; j++) { // Segundo laço: Grava os produtos que estavam dentro do carrinho dessa venda
                fprintf(f, "%d\n%s\n%f\n%d\n%f\n", venda[i].itens[j].codigoProduto, venda[i].itens[j].nomeProduto, venda[i].itens[j].precoVenda, venda[i].itens[j].quantidade, venda[i].itens[j].totalItem);
            }
        }
        fclose(f); // Fecha o arquivo para salvar as alterações e liberar a memória do sistema
    }

    // Salvando Pagamentos
    f = fopen("pagamentos.dat", "w"); // abre o arquivo para escrita
    if (f != NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(f, "%d\n", quantidadePagamento); // Grava o número total de pagamentos efetuados
        for (int i = 0; i < quantidadePagamento; i++) { 
            // Salva qual era o número da venda, o valor pago e a sigla do tipo de pagamento (d/c/dc)
            fprintf(f, "%d\n%f\n%s\n", pagamento[i].numVenda, pagamento[i].valorPago, pagamento[i].tipo);
        }
        fclose(f); // Fecha o arquivo para salvar as alterações e liberar a memória do sistema
    }
}

// Carregando Informação já salva
/* Le os dados salvos anteriormente dos arquivos.*/
void carregarInformacao() {
    FILE *f; // ponteiro para arquivo

    f = fopen("usuarios.dat", "r"); // abre para leitura
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeUsuario);  // le a quantidade salva
        usuario = malloc(quantidadeUsuario * sizeof(Usuario)); // aloca memoria
        for (int i = 0; i < quantidadeUsuario; i++) {
            fscanf(f, " %[^\n] %[^\n] %d", usuario[i].login, usuario[i].senha, &usuario[i].tipo);
        }
        fclose(f);
    }

    f = fopen("categorias.dat", "r"); // abre para leitura
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeCategoria);  // le a quantidade salva
        categoria = malloc(quantidadeCategoria * sizeof(Categoria)); // aloca memoria
        for (int i = 0; i < quantidadeCategoria; i++) {
            fscanf(f, "%d %[^\n]", &categoria[i].codigo, categoria[i].nome);
        }
        fclose(f);
    }

    f = fopen("clientes.dat", "r"); // abre para leitura
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeCliente);  // le a quantidade salva
        cliente = malloc(quantidadeCliente * sizeof(Clientes)); // aloca memoria
        for (int i = 0; i < quantidadeCliente; i++) {
            fscanf(f, "%d %[^\n] %[^\n] %[^\n] %[^\n] %[^\n] %[^\n]", &cliente[i].codigo, cliente[i].nomeCompleto, cliente[i].nomeSocial, cliente[i].cPF, cliente[i].rua_numero, cliente[i].bairro, cliente[i].celular);
        }
        fclose(f);
    }

    f = fopen("produtos.dat", "r"); // abre para leitura
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeProduto);  // le a quantidade salva
        produto = malloc(quantidadeProduto * sizeof(Produtos)); // aloca memoria
        for (int i = 0; i < quantidadeProduto; i++) {
            fscanf(f, "%d %[^\n] %[^\n] %f %f %f %d %d", &produto[i].codigo, produto[i].nomeProduto, produto[i].categoria, &produto[i].precoCompra, &produto[i].margemLucro, &produto[i].precoVenda, &produto[i].estoque, &produto[i].estoqueMinimo);
        }
        fclose(f);
    }

    f = fopen("vendas.dat", "r"); // abre para leitura
    if (f != NULL) {
        fscanf(f, "%d", &quantidadeVenda);  // le a quantidade salva
        venda = malloc(quantidadeVenda * sizeof(Venda)); // aloca memoria
        for (int i = 0; i < quantidadeVenda; i++) {
            fscanf(f, "%d %d %d %d %d %d %f %f %f %c", &venda[i].numVenda, &venda[i].codigoCliente, &venda[i].dia, &venda[i].mes, &venda[i].ano, &venda[i].numItens, &venda[i].totalCarrinho, &venda[i].desconto, &venda[i].totalFinal, &venda[i].status);
            for (int j = 0; j < venda[i].numItens; j++) {
                fscanf(f, "%d %[^\n] %f %d %f", &venda[i].itens[j].codigoProduto, venda[i].itens[j].nomeProduto, &venda[i].itens[j].precoVenda, &venda[i].itens[j].quantidade, &venda[i].itens[j].totalItem);
            }
        }
        fclose(f);
    }

    f = fopen("pagamentos.dat", "r"); // abre para leitura
    if (f != NULL) {
        fscanf(f, "%d", &quantidadePagamento);  // le a quantidade salva
        pagamento = malloc(quantidadePagamento * sizeof(Pagamento)); // aloca memoria
        for (int i = 0; i < quantidadePagamento; i++) {
            fscanf(f, "%d %f %[^\n]", &pagamento[i].numVenda, &pagamento[i].valorPago, pagamento[i].tipo);
        }
        fclose(f);
    }
}
