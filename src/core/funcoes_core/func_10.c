// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

void func_10(char* arquivoBin, char* arquivoIndice, int n){

    REG_DADOS_STRUCT* registros_de_busca = NULL;
    int* mask = NULL;
    FILE* fpDados = NULL;
    FILE* fpIndice = NULL;

    fpDados = abre_binario(arquivoBin, true);
    if(fpDados == NULL){
        DEBUG("ERRO EM func_10: NÃO CONSEGUIU ABRIR O BINÁRIO DE DADOS %s.\n", arquivoBin);
        goto erro;
    }

    fpIndice = abrir_indice(arquivoIndice, true);
    if(fpIndice == NULL){
        DEBUG("ERRO EM func_10: NÃO CONSEGUIU ABRIR O ARQUIVO DE ÍNDICE %s.\n", arquivoIndice);
        goto erro;
    }

    registros_de_busca = (REG_DADOS_STRUCT*)malloc(n*sizeof(REG_DADOS_STRUCT));
    mask = (int*)calloc(n, sizeof(int));
    if(registros_de_busca == NULL || mask == NULL){
        DEBUG("ERRO EM func_10: NÃO CONSEGUIU ALOCAR MEMÓRIA PARA OS REGISTROS DE BUSCA OU O MASK.\n");
        fclose(fpIndice);
        if(registros_de_busca) free(registros_de_busca);
        if(mask) free(mask);
        fecha_binario(fpDados);
        goto erro;
    }

    for(int i = 0; i < n; i++){
        ler_campos(&registros_de_busca[i], &mask[i]);
    }

    int topoPilha, proxRRN;
    fseek(fpDados, 1, SEEK_SET);
    if(fread(&topoPilha, 4, 1, fpDados) != 1 || fread(&proxRRN, 4, 1, fpDados) != 1){
        DEBUG("ERRO EM func_10: NÃO CONSEGUIU LER O TOPO DA PILHA OU O proxRRN NO CABEÇALHO DO BINÁRIO DE DADOS.\n");
        fclose(fpIndice);
        free(registros_de_busca);
        free(mask);
        fecha_binario(fpDados);
        goto erro;
    }

    unsigned char removido_flag = '1';

    for(int i = 0; i < n; i++){
        // Se a busca envolve a chave primária (codEstacao), usa o índice Árvore-B
        if(mask[i] & 1){
            DEBUG("Buscando registro com codEstacao %d usando o índice Árvore-B.\n", registros_de_busca[i].codEstacao);
            int byteOffset = buscar_entrada(fpIndice, registros_de_busca[i].codEstacao);
            if(byteOffset != -1){
                DEBUG("Registro encontrado no índice Árvore-B com byteOffset %d. Verificando se o registro corresponde aos outros campos de busca...\n", byteOffset);
                int rrnIndice = (byteOffset - HEADER_S) / REG_DADOS_S;  // Conversão "byte offset" para "RRN"
                if(check_registro(&registros_de_busca[i], mask[i], rrnIndice, fpDados)){
                    DEBUG("Registro encontrado no índice Árvore-B corresponde aos outros campos de busca. Removendo...\n");
                    
                    fseek(fpDados, HEADER_S + rrnIndice * REG_DADOS_S, SEEK_SET);
                    fwrite(&removido_flag, 1, 1, fpDados);
                    fwrite(&topoPilha, 4, 1, fpDados);
                    topoPilha = rrnIndice;

                    remover_chave_arvoreB(fpIndice, registros_de_busca[i].codEstacao);
                }
            }
        } else {
            DEBUG("Busca não envolve a chave primária. Realizando busca sequencial no arquivo de dados.\n");
            // Caso contrário, faz busca sequencial no arquivo de dados
            for(int RRN = 0; RRN < proxRRN; RRN++){
                DEBUG("RRN = %d, proxRRN = %d\n", RRN, proxRRN);
                if(check_registro(&registros_de_busca[i], mask[i], RRN, fpDados)){
                    REG_DADOS_STRUCT registro_remocao;
                    fseek(fpDados, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                    DEBUG("Verificando registro no RRN %d.\n", RRN);
                    // Precisamos ler o registro para saber qual chave deletar da Árvore-B
                    if(load_registro(fpDados, &registro_remocao)){
                        // Primeiro marca o registro de dados como logicamente removido.
                        DEBUG("Registro no RRN %d corresponde aos campos de busca. Removendo...\n", RRN);
                        fseek(fpDados, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                        fwrite(&removido_flag, 1, 1, fpDados);
                        fwrite(&topoPilha, 4, 1, fpDados);
                        topoPilha = RRN;

                        // Depois remove a entrada do índice Árvore-B.
                        if(registro_remocao.codEstacao != -1){ // codEstacao não pode ser -1 pois esse é o valor usado para indicar "não há chave" na árvore-B
                            DEBUG("Registro no RRN %d corresponde aos campos de busca. Removendo chave %d da Árvore-B.\n", RRN, registro_remocao.codEstacao);
                            remover_chave_arvoreB(fpIndice, registro_remocao.codEstacao);
                        }
                        if(registro_remocao.nomeEstacao) free(registro_remocao.nomeEstacao);
                        if(registro_remocao.nomeLinha) free(registro_remocao.nomeLinha);
                    }
                    else{
                        DEBUG("ERRO EM func_10: FALHA AO CARREGAR O REGISTRO DO RRN %d PARA VERIFICAÇÃO.\n", RRN);
                    }
                }
            }
        }

        if(mask[i] & 64) free(registros_de_busca[i].nomeEstacao);
        if(mask[i] & 128) free(registros_de_busca[i].nomeLinha);
    }

    atualizar_cabecalho(fpDados, topoPilha, proxRRN);

    if(fecha_binario(fpDados) != 0){
        DEBUG("ERRO EM func_10: ERRO AO FECHAR BIN %s\n", arquivoBin);
        goto erro;
    }

    if(fechar_indice(fpIndice, true) == false){
        DEBUG("ERRO EM func_10: ERRO AO FECHAR INDICE %s\n", arquivoIndice);
        goto erro;
    }
    free(registros_de_busca);
    free(mask);

    BinarioNaTela(arquivoBin);
    BinarioNaTela(arquivoIndice);

    return;

    erro:

    printf("Falha no processamento do arquivo.\n");
    free(registros_de_busca);
    free(mask);
    fecha_binario(fpDados);
    fechar_indice(fpIndice, true);
}