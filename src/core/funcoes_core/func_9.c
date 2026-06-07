// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

void func_9(char* arquivoBin, char* arquivoIndice, int n){
    REG_DADOS_STRUCT* registros_de_busca = NULL;
    int* mask = NULL;

    FILE* filestream_bin = abre_binario(arquivoBin, true);
    if(filestream_bin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE* fpIndice = abre_binario(arquivoIndice, true);
    if(fpIndice == NULL){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(filestream_bin);
        return;
    }

    char status;
    if(fread(&status, 1, 1, fpIndice) != 1 || status != '1'){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(filestream_bin);
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
        fecha_binario(filestream_bin);
        return;
    }

    for(int i=0;i<n;i++){
        ler_campos(&registros_de_busca[i], &mask[i]);
    }

    int topoPilha, proxRRN;
    fseek(filestream_bin, 1, SEEK_SET);
    if(fread(&topoPilha, 4, 1, filestream_bin) != 1 || fread(&proxRRN, 4, 1, filestream_bin) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpIndice);
        free(registros_de_busca);
        free(mask);
        fecha_binario(filestream_bin);
        return;
    }

    unsigned char removido_flag = '1';

    for(int i=0;i<n;i++){
        bool encontrou = false;

        if(mask[i] & 1){
            int rrnIndice = buscar_chave(fpIndice, registros_de_busca[i].codEstacao);
            if(rrnIndice != -1){
                if(check_registro(&registros_de_busca[i], mask[i], rrnIndice, filestream_bin)){
                    remover_chave_arvoreB(fpIndice, registros_de_busca[i].codEstacao);
                    fseek(filestream_bin, HEADER_S + rrnIndice * REG_DADOS_S, SEEK_SET);
                    fwrite(&removido_flag, 1, 1, filestream_bin);
                    fwrite(&topoPilha, 4, 1, filestream_bin);
                    topoPilha = rrnIndice;
                    encontrou = true;
                }
            }
        } else {
            for(int RRN = 0; RRN < proxRRN; RRN++){
                if(check_registro(&registros_de_busca[i], mask[i], RRN, filestream_bin)){
                    REG_DADOS_STRUCT registro_remocao;
                    fseek(filestream_bin, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                    if(load_registro(filestream_bin, &registro_remocao)){
                        if(registro_remocao.codEstacao != -1){
                            remover_chave_arvoreB(fpIndice, registro_remocao.codEstacao);
                        }
                        if(registro_remocao.nomeEstacao) free(registro_remocao.nomeEstacao);
                        if(registro_remocao.nomeLinha) free(registro_remocao.nomeLinha);
                    }

                    fseek(filestream_bin, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                    fwrite(&removido_flag, 1, 1, filestream_bin);
                    fwrite(&topoPilha, 4, 1, filestream_bin);
                    topoPilha = RRN;
                    encontrou = true;
                }
            }
        }

        if (!encontrou) printf("Registro inexistente.\n");

        if(mask[i] & 64) free(registros_de_busca[i].nomeEstacao);
        if(mask[i] & 128) free(registros_de_busca[i].nomeLinha);

    }

    if(fecha_binario(filestream_bin) != 0){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpIndice);
        free(registros_de_busca);
        free(mask);
        return;
    }
    atualizar_cabecalho(arquivoBin, topoPilha, proxRRN);

    fclose(fpIndice);
    free(registros_de_busca);
    free(mask);

    BinarioNaTela(arquivoBin);
    BinarioNaTela(arquivoIndice);
}
