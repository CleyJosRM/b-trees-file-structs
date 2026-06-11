#include "core/definicoes.h"
#include "core/datamanager.h"

void func_7(char* arquivoDados, char* arquivoIndice) {
    FILE* fpDados = abre_binario(arquivoDados, false);
    if (!fpDados) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE* fpIndice = fopen(arquivoIndice, "wb+");
    if (!fpIndice) {
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpDados);
        return;
    }

    criar_indice(fpIndice); // Cria a árvore B (arquivo de índice)

    fseek(fpDados, HEADER_S, SEEK_SET);
    REG_DADOS_STRUCT registro;
    int rrn = 0;

    while (load_registro(fpDados, &registro)) {
        if (registro.removido == '0' && registro.codEstacao != -1) {
            inserir_entrada(fpIndice, registro.codEstacao, HEADER_S + rrn * REG_DADOS_S);        }

        free(registro.nomeEstacao);
        free(registro.nomeLinha);
        rrn++;
    }

    fseek(fpIndice, 0, SEEK_SET);
    char status = '1';
    fwrite(&status, 1, 1, fpIndice);

    fecha_binario(fpDados);
    fclose(fpIndice);

    BinarioNaTela(arquivoIndice);

    #ifdef PRINT_ERROS
    ExibirBinario(arquivoIndice);
    #endif

    return;
}