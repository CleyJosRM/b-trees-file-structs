// Cleyton Jose Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "arvoreb/arvoreb_interna.h"

// Dado um nome de arquivo e modo, abre o arquivo de índice, tratando o status conforme especificação.
FILE* abrir_indice(char* nomeIndice, char* modo){  

    // Verificando o modo correto:

    if(modo == NULL || modo_eh_valido(modo) == false){
        DEBUG("ERRO EM abrir_indice: NÃO É UM MODO VÁLIDO.\n");
        return NULL;
    }

    // Abrindo o arquivo

    FILE* fs = fopen(nomeIndice, modo);
    if(fs == NULL){
        DEBUG("ERRO EM abrir_indice: NÃO ENCONTROU O ARQUIVO. VERIFIQUE NOME E PERMISSÕES.\n");
        return NULL;
    }

    // Verificando se o status está consistente:

    byteBTree status;
    if(strcmp(modo, "wb+") != 0){
        fseek(fs, 0, SEEK_SET);
        fread(&status, 1, 1, fs);
        if(status == INCONSISTENTE){ // se estiver inconsistente, fecha o arquivo e retorna nulo.
            if(fclose(fs) != 0){
                DEBUG("ERRO EM abrir_indice: ERRO AO USAR fclose APÓS CONFIRMAR INCONSISTÊNCIA.\n");
            }
            return NULL;
        }
    }

    // Se o arquivo for aberto para ser escrito, então o status é marcado como inconsistente

    bool escrita = (strcmp(modo, "rb") == 0) ? false : true; // o único modo que não aceita escrita é rb

    if(escrita){
        status = INCONSISTENTE;
        fseek(fs, 0, SEEK_SET);
        fwrite(&status, 1, 1, fs);
    }

    return fs;    
}

// Dado uma filestream e um modo, fecha o arquivo tratando status conforme especificação.
bool fechar_indice(FILE* indice, char* modo){
    if(indice == NULL){
        return true;
    }

    bool marcarConsistente = (strcmp(modo, "rb") == 0) ? false : true; // se o modo foi aberto para leitura, o status não deve ser alterado

    // Marcando o status do arquivo como consistente

    if(marcarConsistente){
        fseek(indice, 0, SEEK_SET);
        byteBTree status = CONSISTENTE;
        fwrite(&status, 1, 1, indice);
    }

    // Fechando o índice

    if(fclose(indice) != 0){
        DEBUG("ERRO EM fechar_indice: ERRO AO USAR fclose NO ARQUIVO DE ÍNDICE.\n");
        return false;
    }

    return true;
}

// Preenche os primeiros bytes do arquivo com o cabeçalho, inicializando os campos
void criar_indice(FILE* arquivo){

    // Preenchendo o cabeçalho com valores iniciais

    byteBTree cabecalhoInicial[TAM_CABECALHO_BTREE]; // alocando buffer na memória
    cabecalhoInicial[BO_status] = INCONSISTENTE; // status inconsistente pois serão realizadas escritas
    set_inteiro(cabecalhoInicial, BO_RRNraiz, -1); // não há raiz
    set_inteiro(cabecalhoInicial, BO_topo, -1); // não há registros removidos
    set_inteiro(cabecalhoInicial, BO_proxRRN, 0); // próximo RRN é 0
    set_inteiro(cabecalhoInicial, BO_nroNos, 0); // não há nós

    // Escrevendo no disco:

    fseek(arquivo, 0, SEEK_SET); // voltando ao início do arquivo para escrever o cabeçalho
    fwrite(cabecalhoInicial, TAM_CABECALHO_BTREE, 1, arquivo); // escrevendo o cabeçalho
}

// Documentação em arvoreb_interna.h
int criar_no(byteBTree* novoNo, FILE* arvoreB, byteBTree* cabecalho, int tipoNo){
    
    // Inicializando valores do nó:

    novoNo[BO_removido] = '0'; // o novo nó não está removido
    set_inteiro(novoNo, BO_proximo, -1); // como não está removido, o RRN do próximo removido deve ser -1
    set_inteiro(novoNo, BO_tipoNo, tipoNo); // o tipo do nó é especificado pelo chamador
    set_entradas(novoNo, NULL, 0); // inicializando o nó com entradas nulas
    set_inteiro(novoNo, BO_P1, -1); // inicializando P1 como -1
    set_inteiro(novoNo, BO_nroChaves, 0); // o nroChaves do nó inicialmente é 0

    int RRNnovoNo;
    int topo = get_inteiro(cabecalho, BO_topo);
    if(topo == -1){ // se não há nenhum nó removido
        
        // Obtendo RRN do novo nó e atualizando cabeçalho:
        
        RRNnovoNo = get_inteiro(cabecalho, BO_proxRRN); // lendo o cabeçalho para obter o RRN do novo nó 
        inc_inteiro(cabecalho, BO_proxRRN, 1); // atualizando o proxRRN no cabeçalho
    
    }else{ // se há algum nó removido

        RRNnovoNo = topo; // o RRN do novo nó é o RRN do nó no topo da pilha de removidos

        byteBTree noTopo[TAM_NO_BTREE]; // buffer na memória para segurar o nó do topo
        carregar_no(noTopo, arvoreB, topo); // trazendo o nó removido do disco para a memória
        int RRNproximoRemovido = get_inteiro(noTopo, BO_proximo); // obtendo o RRN do próximo na pilha

        set_inteiro(cabecalho, BO_topo, RRNproximoRemovido); // atualizando o topo da pilha com o RRN do próximo na pilha

    }

    inc_inteiro(cabecalho, BO_nroNos, 1); // atualizando o nroNos no cabeçalho  

    return RRNnovoNo;
    
}