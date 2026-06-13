// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_5(FILE* arquivoBin, int n){

    // LENDO E INSERINDO CADA REGISTRO, CONFORME A PILHA DE REMOVIDOS

    int topo, proxRRN;
    // Lê o topo da pilha de removidos
    fseek(arquivoBin, 1, SEEK_SET);
    fread(&topo, 4, 1, arquivoBin);
    // Lê o próximo RRN disponível para o fim do arquivo
    fread(&proxRRN, 4, 1, arquivoBin);

    for(int i = 0; i < n; i++){
        REG_DADOS_STRUCT* registro_lido = ler_input_reg();
        if(registro_lido == NULL){
            DEBUG("ERRO EM func_5: NÃO FOI POSSÍVEL OBTER VALORES DO USUÁRIO PARA O REGISTRO.\n"); continue;
        }

        if(topo != -1){
            // Reutilização do espaço na pilha de removidos
            long offset = (long)topo * REG_DADOS_S + HEADER_S;
            fseek(arquivoBin, offset + 1, SEEK_SET); // Pula o byte 'removido' para ler o próximo da pilha
            
            int proximo_na_pilha;
            fread(&proximo_na_pilha, 4, 1, arquivoBin);

            // Volta para o início do registro para escrever os novos dados
            fseek(arquivoBin, offset, SEEK_SET);
            if(escreve_registro(registro_lido, arquivoBin) == false){
                DEBUG("ERRO EM func_5: FALHA EM ESCREVER REGISTRO NO ARQUIVO. codEstacao = %d.\n", registro_lido->codEstacao);
            }

            // Atualiza o topo da pilha
            topo = proximo_na_pilha;
        }else{
            // Usa o proxRRN para inserir no fim
            long offset = (long)proxRRN * REG_DADOS_S + HEADER_S;
            fseek(arquivoBin, offset, SEEK_SET);
            if(escreve_registro(registro_lido, arquivoBin) == false){
                DEBUG("ERRO EM func_5: FALHA EM ESCREVER REGISTRO NO ARQUIVO. codEstacao = %d.\n", registro_lido->codEstacao);
            }
            
            proxRRN++; // Incrementa o contador de registros do arquivo
        }

        // Limpeza de memória do registro lido
        if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
        if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
        free(registro_lido);
    }

    // ATUALIZANDO CABEÇALHO
    
    atualizar_cabecalho(arquivoBin, topo, proxRRN);    

    // RETORNANDO

    return true;

}