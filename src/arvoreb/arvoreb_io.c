#include "../include/arvoreb/arvoreb_interna.h"

// Macros de Offset
#define BYTEOFFSET_STATUS 0
#define BYTEOFFSET_RRNraiz 1
#define BYTEOFFSET_TOPO 5
#define BYTEOFFSET_PROXRRN 9
#define BYTEOFFSET_NRONOS 13

#define BYTEOFFSET_REMOVIDO 0
#define BYTEOFFSET_PROXPILHA 1
#define BYTEOFFSET_TIPONO 5
#define BYTEOFFSET_NROCHAVES 9
#define BYTEOFFSET_C1 13
#define BYTEOFFSET_P1 37

int get_RRNraiz(byteNoB* cabecalho){
    return *((int*)&(cabecalho[BYTEOFFSET_RRNraiz]));
}

int get_topo(byteNoB* cabecalho){
    return *((int*)&(cabecalho[BYTEOFFSET_TOPO]));	
}

int get_proxRRN(byteNoB* cabecalho){
    return *((int*)&(cabecalho[BYTEOFFSET_PROXRRN]));	
}

int get_nroNos(byteNoB* cabecalho){
    return *((int*)&(cabecalho[BYTEOFFSET_NRONOS]));
}

void armazenar_no(FILE* arvoreB, int RRN, byteNoB* no){
    fseek(arvoreB, TAM_CABECALHO_BTREE + TAM_NO_BTREE * RRN, SEEK_SET);
    fwrite(no, TAM_NO_BTREE, 1, arvoreB);
}

void carregar_no(FILE* arvoreB, int RRN, byteNoB* buffer){
    fseek(arvoreB, TAM_CABECALHO_BTREE + TAM_NO_BTREE * RRN, SEEK_SET);
    fread(buffer, TAM_NO_BTREE, 1, arvoreB);
}

void armazenar_cabecalho(FILE* arvoreB, byteNoB* buffer){
    fseek(arvoreB, 0, SEEK_SET);
    fwrite(buffer, TAM_CABECALHO_BTREE, 1, arvoreB);
}

void carregar_cabecalho(FILE* arvoreB, byteNoB* buffer, bool statusInconsistente){
    if(arvoreB == NULL || buffer == NULL){
        printf("Erro em carregar_cabecalho!\n");
        exit(1);
    }

    if(statusInconsistente){
        fseek(arvoreB, 0, SEEK_SET);
        char status = '0';
        fwrite(&status, 1, 1, arvoreB);
        fread(buffer+1, TAM_CABECALHO_BTREE-1, 1, arvoreB);
        buffer[0] = '0';
        armazenar_cabecalho(arvoreB, buffer);
    }else{
        fseek(arvoreB, 0, SEEK_SET);
        fread(buffer, TAM_CABECALHO_BTREE, 1, arvoreB);
    }
}

void get_entradas(ENTRADA_INDICE* vetorEntradas, int n, byteNoB* no){
    for(int i = 0; i < n; i++){
        vetorEntradas[i].chave = no_obter_chave(no, i);
        vetorEntradas[i].RRNdados = no_obter_RRNdados(no, i);
        vetorEntradas[i].RRNdescendente = no_obter_filho(no, i + 1); // P_i é o filho à direita (i+1)
    }
}

void set_entradas(ENTRADA_INDICE* vetorEntradas, int n, byteNoB* no){
    for(int i = 0; i < n; i++){
        no_definir_chave(no, i, vetorEntradas[i].chave);
        no_definir_RRNdados(no, i, vetorEntradas[i].RRNdados);
        no_definir_filho(no, i + 1, vetorEntradas[i].RRNdescendente);
    }
    // Limpa os slots não utilizados
    for(int i = n; i < ORDEM_BTREE - 1; i++){
        no_definir_chave(no, i, -1);
        no_definir_RRNdados(no, i, -1);
        no_definir_filho(no, i + 1, -1);
    }
    no_definir_num_chaves(no, n);
}

bool no_eh_folha(byteNoB* no){
    return *(int*)&no[BYTEOFFSET_TIPONO] == TIPOFOLHA;
}

int no_obter_num_chaves(byteNoB* no){
    return *(int*)&no[BYTEOFFSET_NROCHAVES];
}

void no_definir_num_chaves(byteNoB* no, int n){
    *(int*)&no[BYTEOFFSET_NROCHAVES] = n;
}

int no_obter_chave(byteNoB* no, int idx){
    return *(int*)&no[BYTEOFFSET_C1 + 8*idx];
}

void no_definir_chave(byteNoB* no, int idx, int chave){
    *(int*)&no[BYTEOFFSET_C1 + 8*idx] = chave;
}

int no_obter_RRNdados(byteNoB* no, int idx){
    return *(int*)&no[BYTEOFFSET_C1 + 8*idx + 4];
}

void no_definir_RRNdados(byteNoB* no, int idx, int rrn){
    *(int*)&no[BYTEOFFSET_C1 + 8*idx + 4] = rrn;
}

int no_obter_filho(byteNoB* no, int idx){
    return *(int*)&no[BYTEOFFSET_P1 + 4*idx];
}

void no_definir_filho(byteNoB* no, int idx, int rrnFilho){
    *(int*)&no[BYTEOFFSET_P1 + 4*idx] = rrnFilho;
}
