// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "core/func.h"

void func_9(char* arquivoBin, char* arquivoIndice, int n){

    FILE* fpDados = NULL;
    FILE* fpIndice = NULL;

    // Gambiarra: em vez de abrir o arquivo de índice fornecido, vamos criar um do zero a partir do binário de dados

    func_7(arquivoBin, arquivoIndice);

    // Abrindo arquivos:

    fpDados = abre_binario(arquivoBin, true);
    if(fpDados == NULL){
        DEBUG("ERRO EM func_9: NÃO CONSEGUIU ABRIR O BINÁRIO DE DADOS %s.\n", arquivoBin);
        goto erro;
    }
        
    fpIndice = fopen(arquivoIndice, "rb+");
    if(fpIndice == NULL){
        DEBUG("ERRO EM func_9: NÃO CONSEGUIU ABRIR O ARQUIVO DE ÍNDICE %s.\n", arquivoIndice);
        goto erro;
    }

    // Lendo cabeçalho:

    int topo, proxRRN;
    fseek(fpDados, 1, SEEK_SET);
    if(fread(&topo, 4, 1, fpDados) != 1){
        DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER O TOPO DA PILHA NO CABEÇALHO DO BINÁRIO DE DADOS.\n");
        goto erro;
    }
    if(fread(&proxRRN, 4, 1, fpDados) != 1){
        DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER O proxRRN NO CABEÇALHO DO BINÁRIO DE DADOS.\n");
        goto erro;
    }

    // Lendo cada registro do usuário e inserindo:



    for(int i = 0; i < n; i++){
        REG_DADOS_STRUCT* registro_lido = ler_input_reg();
        if(registro_lido == NULL){
            DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER O %d-ÉSIMO REGISTRO DADO PELO USUÁRIO.\n", i+1);
            continue;
        }

        int rrnInserido;
        long offset;

        offset = buscar_entrada(fpIndice, registro_lido->codEstacao);
        if(offset != -1){ // se o registro já estiver no índice, portanto, está no arquivo de dados
            
            printf("O registro com codEstacao %d já existe, está no byteoffset %ld. Substituindo...\n", registro_lido->codEstacao, offset);
            fseek(fpDados, offset, SEEK_SET);
        
        }else if(topo != -1){

            rrnInserido = topo;
            offset = (long)rrnInserido * REG_DADOS_S + HEADER_S;
            fseek(fpDados, offset + 1, SEEK_SET);
            int proximo_na_pilha;
            if(fread(&proximo_na_pilha, 4, 1, fpDados) != 1){
                DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER QUAL O PRÓXIMO NA PILHA.\n");
                if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
                if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
                free(registro_lido);
                goto erro;
            }
            
            topo = proximo_na_pilha;
        
            fseek(fpDados, offset, SEEK_SET);
        
        }else{
        
            rrnInserido = proxRRN;
            offset = (long)rrnInserido * REG_DADOS_S + HEADER_S;
        
            fseek(fpDados, offset, SEEK_SET);
        
            proxRRN++;
        }

        printf("Escrevendo codEstacao %d no byteoffset %ld\n", registro_lido->codEstacao, offset);

        if(escreve_registro(registro_lido, fpDados) == false){
            printf("Falha no processamento do arquivo de dados em escreve_registro.\n");
            goto erro;
        }

        if(registro_lido->codEstacao != -1){
            inserir_entrada(fpIndice, registro_lido->codEstacao, offset);
        }

        if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
        if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
        free(registro_lido);
    }



    fseek(fpDados, 1, SEEK_SET);
    if(fwrite(&topo, 4, 1, fpDados) != 1){
        DEBUG("ERRO EM func_9: NÃO CONSEGUIU ESCREVER O TOPO DE VOLTA NA PILHA");   
        goto erro;
    }

    if(fecha_binario(fpDados) != 0){
        DEBUG("ERRO EM func_9: FALHA AO FECHAR BINÁRIO DE DADOS.\n");
        goto erro;
    }

    atualizar_cabecalho(arquivoBin, topo, proxRRN);

    if(fecha_binario(fpIndice) != 0){
        DEBUG("ERRO EM func_9: FALHA AO FECHAR ARQUIVO DE ÍNDICE.\n");
        goto erro;
    }

    BinarioNaTela(arquivoBin);
    BinarioNaTela(arquivoIndice);

    return;

    erro:

    printf("Falha no processamento do arquivo.\n");
    if(fpIndice != NULL) fclose(fpIndice);
    if(fpDados != NULL) fclose(fpDados);
    return;
}