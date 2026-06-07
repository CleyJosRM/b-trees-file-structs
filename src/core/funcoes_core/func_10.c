// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

static REG_DADOS_STRUCT* ler_input_reg(){
    REG_DADOS_STRUCT* registro_lido = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
    if(registro_lido == NULL) return NULL;

    registro_lido->removido = '0';
    registro_lido->proximo = -1;

    char buffer[100];

    ScanQuoteString(buffer);
    registro_lido->codEstacao = processar_int(buffer);

    ScanQuoteString(buffer);
    registro_lido->nomeEstacao = processar_string(buffer, &registro_lido->tamNomeEstacao);

    ScanQuoteString(buffer);
    registro_lido->codLinha = processar_int(buffer);

    ScanQuoteString(buffer);
    registro_lido->nomeLinha = processar_string(buffer, &registro_lido->tamNomeLinha);

    ScanQuoteString(buffer);
    registro_lido->codProxEstacao = processar_int(buffer);

    ScanQuoteString(buffer);
    registro_lido->distProxEstacao = processar_int(buffer);

    ScanQuoteString(buffer);
    registro_lido->codLinhaIntegra = processar_int(buffer);

    ScanQuoteString(buffer);
    registro_lido->codEstIntegra = processar_int(buffer);

    return registro_lido;
}

void func_10(char* arquivoBin, char* arquivoIndice, int n){
    FILE* fpDados = abre_binario(arquivoBin, true);
    if(fpDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE* fpIndice = abre_binario(arquivoIndice, true);
    if(fpIndice == NULL){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpDados);
        return;
    }

    char status;
    if(fread(&status, 1, 1, fpIndice) != 1 || status != '1'){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpIndice);
        fecha_binario(fpDados);
        return;
    }

    int topo, proxRRN;
    fseek(fpDados, 1, SEEK_SET);
    if(fread(&topo, 4, 1, fpDados) != 1 || fread(&proxRRN, 4, 1, fpDados) != 1){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpIndice);
        fecha_binario(fpDados);
        return;
    }

    for(int i = 0; i < n; i++){
        REG_DADOS_STRUCT* registro_lido = ler_input_reg();
        if(registro_lido == NULL){
            printf("Falha no processamento do arquivo.\n");
            continue;
        }

        int rrnInserido;

        if(topo != -1){
            rrnInserido = topo;
            long offset = (long)topo * REG_DADOS_S + HEADER_S;
            fseek(fpDados, offset + 1, SEEK_SET);
            int proximo_na_pilha;
            if(fread(&proximo_na_pilha, 4, 1, fpDados) != 1){
                printf("Falha no processamento do arquivo.\n");
                if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
                if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
                free(registro_lido);
                fecha_binario(fpIndice);
                fecha_binario(fpDados);
                return;
            }
            topo = proximo_na_pilha;
            fseek(fpDados, offset, SEEK_SET);
        } else {
            rrnInserido = proxRRN;
            long offset = (long)proxRRN * REG_DADOS_S + HEADER_S;
            fseek(fpDados, offset, SEEK_SET);
            proxRRN++;
        }

        if(escreve_registro(registro_lido, fpDados) == false){
            printf("Falha no processamento do arquivo.\n");
        }

        if(registro_lido->codEstacao != -1){
            inserir_entrada_na_arvore(fpIndice, registro_lido->codEstacao, rrnInserido);
        }

        if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
        if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
        free(registro_lido);
    }

    if(fecha_binario(fpDados) != 0){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpIndice);
        return;
    }

    atualizar_cabecalho(arquivoBin, topo, proxRRN);

    if(fecha_binario(fpIndice) != 0){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    BinarioNaTela(arquivoBin);
    BinarioNaTela(arquivoIndice);
}
