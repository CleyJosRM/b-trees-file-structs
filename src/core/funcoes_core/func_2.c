// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_2(FILE* arquivoBin){

	fseek(arquivoBin, HEADER_S, SEEK_SET);

	unsigned char removido;
	bool reg_existe = false;

	while(fread(&removido, 1, 1, arquivoBin) == 1){
		if(removido == '1'){ // O registro foi removido, salta o registro ao invés de ler
			fseek(arquivoBin, REG_DADOS_S - 1, SEEK_CUR); // -1 pois o cursor está no segundo byte
		}else{
            fseek(arquivoBin, -1, SEEK_CUR);
			print_registro(arquivoBin);
			reg_existe = true;	
		}
	}

    if (!reg_existe) {
        printf("Registro inexistente.\n");
    }

	return true;
}