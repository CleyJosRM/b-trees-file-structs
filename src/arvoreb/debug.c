#include "../../include/arvoreb/arvoreb_interna.h"

/*
FUNÇÕES PARA DEBUG
imprimem os nós e cabeçalho com formatação para facilitar a leitura
ou os bytes brutos para podermos inspecionar o estado interno do nó
*/





// Imprime em hexadecimal os 53 bytes do nó, com quebras de linha para facilitar a leitura.
void imprimir_no_bytes(byteBTree* no){
	for(int i=0; i<TAM_NO_BTREE; i++){
		printf("%2x ", no[i]); // imprime o byte
		if(i%10 == 9){
			printf("\n"); // se já foram impressos 10, 20, 30, 40, 50 bytes, quebra a linha
		}
	}
	printf("\n");
}

// Imprime os valores contidos em um nó de maneira formatada para facilitar a interpretação.
void imprimir_no(byteBTree* noB, int RRN){
	printf("\n--- NÓ RRN = %d ---\n", RRN);
	imprimir_no_bytes(noB);
    printf("removido: %c | próximo: %d | tipoNo: %d | nroChaves: %d\n",
			noB[0],
            get_inteiro(noB, BO_proximo),
            get_inteiro(noB, BO_tipoNo),
            get_inteiro(noB, BO_nroChaves));

    printf("  Chaves: C1: %d, C2: %d, C3: %d\n",
			get_inteiro(noB, BO_C1),
			get_inteiro(noB, BO_C1 + 8),
            get_inteiro(noB, BO_C1 + 16));

    printf("  RRN dados: PR1: %d, PR2: %d, PR3: %d\n",
            get_inteiro(noB, BO_PR1),
            get_inteiro(noB, BO_PR1 + 8),
            get_inteiro(noB, BO_PR1 + 16));

    printf("  RRN descendentes: P1: %d, P2: %d, P3: %d, P4: %d\n",
            get_inteiro(noB, BO_P1),
            get_inteiro(noB, BO_P1+4),
            get_inteiro(noB, BO_P1+8),
            get_inteiro(noB, BO_P1+12));
}

// Imprime os valores contidos no cabeçalho de maneira formatada para facilitar a interpretação.
void imprimir_cabecalho(byteBTree* cabecalho){
	printf("status: %c\nnoRaiz: %d\ntopo: %d\nproxRRN: %d\nnroNos: %d\n", 
           cabecalho[0], 
           get_inteiro(cabecalho, BO_RRNraiz), 
           get_inteiro(cabecalho, BO_topo), 
           get_inteiro(cabecalho, BO_proxRRN), 
           get_inteiro(cabecalho, BO_nroNos));
}
