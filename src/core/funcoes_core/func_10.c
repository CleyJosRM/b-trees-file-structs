// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

void func_10(char* arquivoBin, char* arquivoIndice, int n){
    REG_DADOS_STRUCT* registros_de_busca = NULL;
    int* mask = NULL;

    FILE* fpDados = abre_binario(arquivoBin, true);
    if(fpDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE* fpIndice = fopen(arquivoIndice, "rb+");
    if(fpIndice == NULL){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpDados);
        return;
    }

    char status;
    if(fread(&status, 1, 1, fpIndice) != 1 || status != '1'){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpDados);
        fclose(fpIndice);
        return;
    }

    registros_de_busca = (REG_DADOS_STRUCT*)malloc(n*sizeof(REG_DADOS_STRUCT));
    mask = (int*)calloc(n, sizeof(int));
    if(registros_de_busca == NULL || mask == NULL){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpIndice);
        if(registros_de_busca) free(registros_de_busca);
        if(mask) free(mask);
        fecha_binario(fpDados);
        return;
    }

    for(int i = 0; i < n; i++){
        ler_campos(&registros_de_busca[i], &mask[i]);
    }

    int topoPilha, proxRRN;
    fseek(fpDados, 1, SEEK_SET);
    if(fread(&topoPilha, 4, 1, fpDados) != 1 || fread(&proxRRN, 4, 1, fpDados) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpIndice);
        free(registros_de_busca);
        free(mask);
        fecha_binario(fpDados);
        return;
    }

    unsigned char removido_flag = '1';

    for(int i = 0; i < n; i++){
        // Se a busca envolve a chave primária (codEstacao), usa o índice Árvore-B
        if(mask[i] & 1){
            int byteOffset = buscar_chave(fpIndice, registros_de_busca[i].codEstacao);
            if(byteOffset != -1){
                int rrnIndice = (byteOffset - HEADER_S) / REG_DADOS_S;  // Conversão "byte offset" para "RRN"
                if(check_registro(&registros_de_busca[i], mask[i], rrnIndice, fpDados)){
                    remover_chave_arvoreB(fpIndice, registros_de_busca[i].codEstacao);
                    
                    fseek(fpDados, HEADER_S + rrnIndice * REG_DADOS_S, SEEK_SET);
                    fwrite(&removido_flag, 1, 1, fpDados);
                    fwrite(&topoPilha, 4, 1, fpDados);
                    topoPilha = rrnIndice;
                }
            }
        } else {
            // Caso contrário, faz busca sequencial no arquivo de dados
            for(int RRN = 0; RRN < proxRRN; RRN++){
                if(check_registro(&registros_de_busca[i], mask[i], RRN, fpDados)){
                    REG_DADOS_STRUCT registro_remocao;
                    fseek(fpDados, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                    
                    // Precisamos ler o registro para saber qual chave deletar da Árvore-B
                    if(load_registro(fpDados, &registro_remocao)){
                        if(registro_remocao.codEstacao != -1){
                            remover_chave_arvoreB(fpIndice, registro_remocao.codEstacao);
                        }
                        if(registro_remocao.nomeEstacao) free(registro_remocao.nomeEstacao);
                        if(registro_remocao.nomeLinha) free(registro_remocao.nomeLinha);
                    }

                    // Volta o ponteiro e atualiza como logicamente removido
                    fseek(fpDados, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                    fwrite(&removido_flag, 1, 1, fpDados);
                    fwrite(&topoPilha, 4, 1, fpDados);
                    topoPilha = RRN;
                }
            }
        }

        if(mask[i] & 64) free(registros_de_busca[i].nomeEstacao);
        if(mask[i] & 128) free(registros_de_busca[i].nomeLinha);
    }

    if(fecha_binario(fpDados) != 0){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpIndice);
        free(registros_de_busca);
        free(mask);
        return;
    }
    
   atualizar_cabecalho(arquivoBin, topoPilha, proxRRN);

    if(fecha_binario(fpIndice) != 0){
        printf("Falha no processamento do arquivo.\n");
        free(registros_de_busca);
        free(mask);
        return;
    }
    free(registros_de_busca);
    free(mask);

    BinarioNaTela(arquivoBin);
    BinarioNaTela(arquivoIndice);
}