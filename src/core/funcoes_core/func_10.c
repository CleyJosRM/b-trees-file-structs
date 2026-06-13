// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_10(FILE* arquivoBin, FILE* arquivoIndice, int n){

    REG_DADOS_STRUCT* registros_de_busca = NULL;
    int* mask = NULL;

    registros_de_busca = (REG_DADOS_STRUCT*)calloc(n, sizeof(REG_DADOS_STRUCT));
    mask = (int*)calloc(n, sizeof(int));
    if(registros_de_busca == NULL || mask == NULL){
        DEBUG("ERRO EM func_10: NÃO CONSEGUIU ALOCAR MEMÓRIA PARA OS REGISTROS DE BUSCA OU O MASK.\n");
        goto erro;
    }

    for(int i = 0; i < n; i++){
        ler_campos(&registros_de_busca[i], &mask[i]);
    }

    int topoPilha, proxRRN;
    fseek(arquivoBin, 1, SEEK_SET);
    if(fread(&topoPilha, 4, 1, arquivoBin) != 1 || fread(&proxRRN, 4, 1, arquivoBin) != 1){
        DEBUG("ERRO EM func_10: NÃO CONSEGUIU LER O TOPO DA PILHA OU O proxRRN NO CABEÇALHO DO BINÁRIO DE DADOS.\n");
        goto erro;
    }

    unsigned char removido_flag = '1';

    for(int i = 0; i < n; i++){
        // Se a busca envolve a chave primária (codEstacao), usa o índice Árvore-B
        if(mask[i] & 1){
            DEBUG("Buscando registro com codEstacao %d usando o índice Árvore-B.\n", registros_de_busca[i].codEstacao);
            int byteOffset = buscar_entrada(arquivoIndice, registros_de_busca[i].codEstacao);
            if(byteOffset != -1){
                DEBUG("Registro encontrado no índice Árvore-B com byteOffset %d. Verificando se o registro corresponde aos outros campos de busca...\n", byteOffset);
                int rrnIndice = (byteOffset - HEADER_S) / REG_DADOS_S;  // Conversão "byte offset" para "RRN"
                if(check_registro(&registros_de_busca[i], mask[i], rrnIndice, arquivoBin)){
                    DEBUG("Registro encontrado no índice Árvore-B corresponde aos outros campos de busca. Removendo...\n");
                    
                    fseek(arquivoBin, HEADER_S + rrnIndice * REG_DADOS_S, SEEK_SET);
                    fwrite(&removido_flag, 1, 1, arquivoBin);
                    fwrite(&topoPilha, 4, 1, arquivoBin);
                    topoPilha = rrnIndice;

                    remover_chave_arvoreB(arquivoIndice, registros_de_busca[i].codEstacao);
                }
            }
        } else {
            DEBUG("Busca não envolve a chave primária. Realizando busca sequencial no arquivo de dados.\n");
            // Caso contrário, faz busca sequencial no arquivo de dados
            for(int RRN = 0; RRN < proxRRN; RRN++){
                DEBUG("RRN = %d, proxRRN = %d\n", RRN, proxRRN);
                if(check_registro(&registros_de_busca[i], mask[i], RRN, arquivoBin)){
                    REG_DADOS_STRUCT registro_remocao;
                    fseek(arquivoBin, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                    DEBUG("Verificando registro no RRN %d.\n", RRN);
                    // Precisamos ler o registro para saber qual chave deletar da Árvore-B
                    if(load_registro(arquivoBin, &registro_remocao)){
                        // Primeiro marca o registro de dados como logicamente removido.
                        DEBUG("Registro no RRN %d corresponde aos campos de busca. Removendo...\n", RRN);
                        fseek(arquivoBin, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                        fwrite(&removido_flag, 1, 1, arquivoBin);
                        fwrite(&topoPilha, 4, 1, arquivoBin);
                        topoPilha = RRN;

                        // Depois remove a entrada do índice Árvore-B.
                        if(registro_remocao.codEstacao != -1){ // codEstacao não pode ser -1 pois esse é o valor usado para indicar "não há chave" na árvore-B
                            DEBUG("Registro no RRN %d corresponde aos campos de busca. Removendo chave %d da Árvore-B.\n", RRN, registro_remocao.codEstacao);
                            remover_chave_arvoreB(arquivoIndice, registro_remocao.codEstacao);
                        }
                        if(registro_remocao.nomeEstacao){ free(registro_remocao.nomeEstacao); registro_remocao.nomeEstacao = NULL; }
                        if(registro_remocao.nomeLinha){ free(registro_remocao.nomeLinha); registro_remocao.nomeEstacao = NULL; }
                    }
                    else{
                        DEBUG("ERRO EM func_10: FALHA AO CARREGAR O REGISTRO DO RRN %d PARA VERIFICAÇÃO.\n", RRN);
                    }
                }
            }
        }
    }

    atualizar_cabecalho(arquivoBin, topoPilha, proxRRN);

    for(int i=0; i<n; i++){
        if(registros_de_busca[i].nomeEstacao != NULL) free(registros_de_busca[i].nomeEstacao);
        if(registros_de_busca[i].nomeLinha != NULL) free(registros_de_busca[i].nomeLinha);
    }
    free(registros_de_busca);
    free(mask);

    return true;

    erro:

    if(registros_de_busca != NULL){
        for(int i=0; i<n; i++){
            if(registros_de_busca[i].nomeEstacao != NULL) free(registros_de_busca[i].nomeEstacao);
            if(registros_de_busca[i].nomeLinha != NULL) free(registros_de_busca[i].nomeLinha);
        }
    }
    free(registros_de_busca);
    free(mask);

    return false;
}