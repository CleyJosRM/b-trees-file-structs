// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"


/**
 * @brief Funcionalidade [5]: Insere novos registros reaproveitando espaços removidos.
 * Simula o comando SQL 'INSERT INTO'. Tenta inserir o novo registro no RRN indicado 
 * pelo 'topo' da pilha de removidos. Caso a pilha esteja vazia (topo == -1), a inserção 
 * ocorre no final do arquivo (proxRRN).
 * @param arquivoBin Nome do arquivo binário para inserção.
 * @param n Quantidade de novos registros a serem inseridos.
 * @return void
 */
void func_5(char* arquivoBin, int n){

    // ABRINDO ARQUIVO
    
    FILE* filestream_bin = abre_binario(arquivoBin, true);
    if(filestream_bin == NULL){
        DEBUG("DEBUG EM func_5: FALHA AO ABRIR O ARQUIVO %s.\n", arquivoBin);
        goto erro;
    }

    // LENDO E INSERINDO CADA REGISTRO, CONFORME A PILHA DE REMOVIDOS

    int topo, proxRRN;
    // Lê o topo da pilha de removidos
    fseek(filestream_bin, 1, SEEK_SET);
    fread(&topo, 4, 1, filestream_bin);
    // Lê o próximo RRN disponível para o fim do arquivo
    fread(&proxRRN, 4, 1, filestream_bin);

    for(int i = 0; i < n; i++){
        REG_DADOS_STRUCT* registro_lido = ler_input_reg();
        if(registro_lido == NULL){
            DEBUG("ERRO EM func_5: NÃO FOI POSSÍVEL OBTER VALORES DO USUÁRIO PARA O REGISTRO.\n"); continue;
        }

        if(topo != -1){
            // Reutilização do espaço na pilha de removidos
            long offset = (long)topo * REG_DADOS_S + HEADER_S;
            fseek(filestream_bin, offset + 1, SEEK_SET); // Pula o byte 'removido' para ler o próximo da pilha
            
            int proximo_na_pilha;
            fread(&proximo_na_pilha, 4, 1, filestream_bin);

            // Volta para o início do registro para escrever os novos dados
            fseek(filestream_bin, offset, SEEK_SET);
            if(escreve_registro(registro_lido, filestream_bin) == false){
                DEBUG("ERRO EM func_5: FALHA EM ESCREVER REGISTRO NO ARQUIVO. codEstacao = %d.\n", registro_lido->codEstacao);
            }

            // Atualiza o topo da pilha
            topo = proximo_na_pilha;
        }else{
            // Usa o proxRRN para inserir no fim
            long offset = (long)proxRRN * REG_DADOS_S + HEADER_S;
            fseek(filestream_bin, offset, SEEK_SET);
            if(escreve_registro(registro_lido, filestream_bin) == false){
                DEBUG("ERRO EM func_5: FALHA EM ESCREVER REGISTRO NO ARQUIVO. codEstacao = %d.\n", registro_lido->codEstacao);
            }
            
            proxRRN++; // Incrementa o contador de registros do arquivo
        }

        // Limpeza de memória do registro lido
        if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
        if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
        free(registro_lido);
    }

    // ATUALIZANDO CABEÇALHO E FECHANDO ARQUIVO
    
    if(fecha_binario(filestream_bin) != 0){
        DEBUG("DEBUG: ERRO AO FECHAR BIN %s\n", arquivoBin);
        goto erro;
    }
    atualizar_cabecalho(arquivoBin, topo, proxRRN);

    // RETORNANDO

    BinarioNaTela(arquivoBin);

    #ifdef PRINT_ERROS
        ExibirBinario(arquivoBin);
    #endif

    return;

    erro:

    if(fecha_binario(filestream_bin) != 0){
        DEBUG("DEBUG: ERRO AO FECHAR BIN %s\n", arquivoBin);
    }
    printf("Falha no processamento do arquivo.\n");
}