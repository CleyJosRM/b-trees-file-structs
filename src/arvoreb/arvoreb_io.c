#include "../../include/arvoreb/arvoreb_interna.h"

/*
FUNÇÕES QUE ESCREVEM E LÊEM DO DISCO
Em todas as funções, é fornecida uma filestream para acessar o arquivo
e o endereço de um buffer na memória onde os bytes serão lidos/escritos.
As funções do tipo "carregar" lêem bytes do disco e os escrevem na memória principal.
As funções do tipo "armazenar" lêem bytes da memória principal e os escrevem no disco.
As permissões da filestream devem ser adequadas e o buffer deve conter espaço suficiente.
*/





/**
 * Transfere um B-nó da memória principal para o arquivo. 
 * A posição onde se deseja inserir esse B-nó deve ser especificada como RRN.
 * A filestream deve permitir escrita.
 */
void armazenar_no(FILE* arvoreB, const byteBTree* no, int RRN){
    fseek(arvoreB, TAM_CABECALHO_BTREE+TAM_NO_BTREE*RRN, SEEK_SET); // calculando byteoffset e posicionando o cursor
    fwrite(no, TAM_NO_BTREE, 1, arvoreB); // escrevendo o conteúdo da memória no disco
}

/**
 * Transfere um B-nó do arquivo para a memória principal. 
 * Para achar esse B-nó, deve ser fornecido o RRN dele.
 * A filestream deve permitir leitura.
 */
void carregar_no(byteBTree* buffer, FILE* arvoreB, int RRN){
    fseek(arvoreB, TAM_CABECALHO_BTREE+TAM_NO_BTREE*RRN, SEEK_SET); // calculando byteoffset e posicionando o cursor
    fread(buffer, TAM_NO_BTREE, 1, arvoreB); // lendo o conteúdo do disco para a memória
}

/**
 * Transfere o cabeçalho da árvore-B da memória principal para o arquivo.
 * A filestream deve permitir escrita.
 */
void armazenar_cabecalho(FILE* arvoreB, const byteBTree* buffer){
    fseek(arvoreB, 0, SEEK_SET); // calculando byteoffset e posicionando o cursor
    fwrite(buffer, TAM_CABECALHO_BTREE, 1, arvoreB); // escrevendo o conteúdo da memória no disco
}

/**
 * Transfere o cabeçalho da árvore-B do arquivo para a memória principal. 
 * Caso statusInconsistente seja true, o status do arquivo é marcado como '0' antes de retornar.
 * A filestream deve permitir leitura e escrita, mas a escrita só acontece quando statusInconsistente é true.
 */
void carregar_cabecalho(byteBTree* buffer, FILE* arvoreB, bool statusInconsistente){
    fseek(arvoreB, 0, SEEK_SET); // calculando byteoffset e posicionando o cursor
    fread(buffer, TAM_CABECALHO_BTREE, 1, arvoreB); // lendo o conteúdo do disco para a memória
    if(statusInconsistente){ // se quisermos marcar o arquivo como inconsistente
        buffer[0] = INCONSISTENTE; // atualizando o status na memória
        fseek(arvoreB, 0, SEEK_SET); // reposicionando o cursor para o primeiro byte
        fwrite(buffer, 1, 1, arvoreB); // atualizando o status no disco
    }
}

int get_RRNraiz(byteBTree* cabecalho){
    return *((int*)&(cabecalho[BO_RRNraiz]));
}

int get_topo(byteBTree* cabecalho){
    return *((int*)&(cabecalho[BO_topo]));  
}

int get_proxRRN(byteBTree* cabecalho){
    return *((int*)&(cabecalho[BO_proxRRN]));   
}

int get_nroNos(byteBTree* cabecalho){
    return *((int*)&(cabecalho[BO_nroNos]));
}

bool no_eh_folha(byteBTree* no){
    return *(int*)&no[BO_tipoNo] == TIPOFOLHA;
}

int get_nroChaves(byteBTree* no){
    return *(int*)&no[BO_nroChaves];
}

void set_nroChaves(byteBTree* no, int n){
    *(int*)&no[BO_nroChaves] = n;
}

int get_chave(byteBTree* no, int idx){
    return *(int*)&no[BO_C1 + 8*idx];
}

void set_chave(byteBTree* no, int idx, int chave){
    *(int*)&no[BO_C1 + 8*idx] = chave;
}

int get_RRNdados(byteBTree* no, int idx){
    return *(int*)&no[BO_C1 + 8*idx + 4];
}

void set_RRNdados(byteBTree* no, int idx, int rrn){
    *(int*)&no[BO_C1 + 8*idx + 4] = rrn;
}

int get_filho(byteBTree* no, int idx){
    return *(int*)&no[BO_P1 + 4*idx];
}

void set_filho(byteBTree* no, int idx, int rrnFilho){
    *(int*)&no[BO_P1 + 4*idx] = rrnFilho;
}
