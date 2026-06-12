// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

void func_7(char* arquivoDados, char* arquivoIndice) {
    
    FILE* fpDados = NULL;
    FILE* fpIndice = NULL;

    // Abrindo arquivos

    fpDados = abre_binario(arquivoDados, false);
    if (!fpDados) {
        DEBUG("ERRO EM func_7: ERRO AO ABRIR O BINÁRIO DE DADOS. PODE ESTAR INCONSISTENTE.\n");
        goto erro;
    }

    fpIndice = fopen(arquivoIndice, "wb+"); // fopen com escrita pois deve-se criar o arquivo, se necessário
    if (!fpIndice) {
        DEBUG("ERRO EM func_7: ERRO AO ABRIR O ÍNDICE. PODE ESTAR INCONSISTENTE.\n");
        goto erro;
    }

    // Criando o índice e inserindo as entradas

    criar_indice(fpIndice); // Preenche o cabeçalho com valores iniciais

    fseek(fpDados, HEADER_S, SEEK_SET); // apontando para a posição do primeiro registro de dados
    REG_DADOS_STRUCT registro;
    int rrn = 0;

    while (load_registro(fpDados, &registro)) { // lendo um registro do arquivo de dados, movendo o file pointer para o próximo
        if (registro.removido == '0' && registro.codEstacao != -1) { // codEstacao não pode ser -1 pois esse é o valor usado para indicar "não há chave" na árvore-B
            inserir_entrada(fpIndice, registro.codEstacao, HEADER_S + rrn * REG_DADOS_S);        }

        free(registro.nomeEstacao); // apagando os nomes
        free(registro.nomeLinha);
        rrn++; // RRN do próximo registro a ser lido (se houver)
    }

    // Fechando arquivos:

    if(fecha_binario(fpDados) != 0){
        DEBUG("ERRO EM func_7: ERRO AO FECHAR BIN %s\n", arquivoBin);
        goto erro;
    }
    if(fechar_indice(fpIndice, true) == false){
        DEBUG("ERRO EM func_7: ERRO AO FECHAR INDICE %s\n", arquivoIndice);
        goto erro;
    }

    BinarioNaTela(arquivoIndice);

    return;

    erro:

    printf("Falha no processamento do arquivo.\n");
    fecha_binario(fpDados);
    fechar_indice(fpIndice, true);

    return;
}