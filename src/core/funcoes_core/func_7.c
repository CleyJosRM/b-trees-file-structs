// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_7(FILE* arquivoDados, FILE* arquivoIndice) {
    
    REG_DADOS_STRUCT registro = {0};

    // Criando o índice e inserindo as entradas

    criar_indice(arquivoIndice); // Preenche o cabeçalho com valores iniciais

    fseek(arquivoDados, HEADER_S, SEEK_SET); // apontando para a posição do primeiro registro de dados
    registro = (REG_DADOS_STRUCT){0};
    int rrn = 0;

    while (load_registro(arquivoDados, &registro)) { // lendo um registro do arquivo de dados, movendo o file pointer para o próximo
        if (registro.removido == '0' && registro.codEstacao != -1) { // codEstacao não pode ser -1 pois esse é o valor usado para indicar "não há chave" na árvore-B
            inserir_entrada(arquivoIndice, registro.codEstacao, HEADER_S + rrn * REG_DADOS_S);
        }

        free(registro.nomeEstacao); // apagando os nomes
        free(registro.nomeLinha);
        registro.nomeEstacao = NULL;
        registro.nomeLinha = NULL;
        rrn++; // RRN do próximo registro a ser lido (se houver)
    }

    return true;

}