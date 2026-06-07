// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#ifndef FUNC_H
	#define FUNC_H
	
		/**
	 * @brief Funcionalidade [1]: Converte um arquivo csv para binário, de acordo com a especificação.
	 * Simula o comando SQL 'CREATE TABLE'. Percorre sequencialmente o arquivo csv e 
	 * e gera os registros de dados correspondentes a cada linha. 
	 * Nenhum registro é marcado como logicamente removido
	 * O cabeçalho é gerado com a contagem correta de estações e de pares (codEstacao, codProxEstacao)
	 * @param arquivoEntrada Nome do arquivo csv de onde os dados serão lidos
	 * @param arquivoEntrada Nome do arquivo binário que será produzido
	 * @return void
	 */
	void func_1(char* arquivoEntrada, char* arquivoSaida);

   /**
	 * @brief Funcionalidade [2]: Recupera e exibe todos os registros do arquivo binário.
	 * Simula o comando SQL 'SELECT FROM'. Percorre sequencialmente o arquivo binário 
	 * e imprime todos os registros que não estão marcados como logicamente removidos. 
	 * Campos nulos são exibidos como 'NULO'.
	 * @param arquivoBin Nome do arquivo binário de onde os dados serão lidos.
	 * @return void
	 */
	void func_2(char* arquivoEntrada);

		/**
	 * @brief Funcionalidade [3]: Recupera registros com base em critérios de busca.
	 * Simula o comando SQL 'SELECT WHERE'. Permite a busca por um ou mais campos 
	 * (inteiros ou strings). Realiza uma busca sequencial no arquivo e exibe todos 
	 * os registros que satisfazem os filtros informados.
	 * @param arquivoBin Nome do arquivo binário para consulta.
	 * @param n Quantidade de buscas independentes a serem realizadas.
	 * @return void
	 */
	void func_3(char* arquivoBin, int n);

		/**
	 * @brief Funcionalidade [4]: Realiza a remoção lógica de registros.
	 * Simula o comando SQL 'DELETE FROM WHERE'. Localiza registros através de filtros 
	 * e os marca como removidos ('1'). Implementa uma lista encadeada (pilha) de espaços 
	 * disponíveis, utilizando o campo 'topo' no cabeçalho para permitir reaproveitamento futuro.
	 * @param arquivoBin Nome do arquivo binário onde ocorrerão as remoções.
	 * @param n Quantidade de operações de remoção a serem processadas.
	 * @return void
	 */
	void func_4(char* arquivoBin, int n);
	
		/**
	 * @brief Funcionalidade [5]: Insere novos registros reaproveitando espaços removidos.
	 * Simula o comando SQL 'INSERT INTO'. Tenta inserir o novo registro no RRN indicado 
	 * pelo 'topo' da pilha de removidos. Caso a pilha esteja vazia (topo == -1), a inserção 
	 * ocorre no final do arquivo (proxRRN).
	 * @param arquivoBin Nome do arquivo binário para inserção.
	 * @param n Quantidade de novos registros a serem inseridos.
	 * @return void
	 */
	void func_5(char* arquivoBin, int n);

		/**
	 * @brief Funcionalidade [6]: Busca por registros e atualiza os campos com novos valores.
	 * Simula o comando SQL 'UPDATE'. Localiza registros através de filtros e atualiza seus campos.
	 * Para cada busca realizada, os registros correspondentes devem ser atualizados 
	 * com os novos valores especificados. Vários ciclos de busca&atualização podem ser realizados.
	 * @param arquivoBin Nome do arquivo binário para atualização.
	 * @param n Quantidade de ciclos de busca&atualização a serem realizados.
	 * @return void
	 */
	void func_6(char* arquivoBin, int n);

	/**
	 * @brief Funcionalidade [7]: Cria um arquivo de índice árvore-B para o arquivo de dados de entrada.
	 * Percorre sequencialmente o arquivo de dados e insere no índice a chave codEstacao de cada registro
	 * não removido. Registros logicamente removidos não são indexados.
	 * @param arquivoDados Nome do arquivo binário de dados de entrada.
	 * @param arquivoIndice Nome do arquivo de índice árvore-B que será criado.
	 * @return void
	 */
	void func_7(char* arquivoDados, char* arquivoIndice);

	/**
	 * @brief Funcionalidade [8]: Recupera registros de acordo com critérios de busca.
	 * Usa o índice árvore-B sempre que a busca incluir o campo codEstacao.
	 * Para buscas sem codEstacao, faz busca sequencial como a funcionalidade 3.
	 * @param arquivoEntrada Nome do arquivo de dados binário.
	 * @param arquivoIndice Nome do arquivo de índice árvore-B.
	 * @param n Quantidade de buscas a serem realizadas.
	 * @return void
	 */
	void func_8(char* arquivoEntrada, char* arquivoIndice, int n);

	/**
	 * @brief Funcionalidade [9]: Remove logicamente registros com base em buscas.
	 * Utiliza o índice B-tree quando a busca é feita por `codEstacao` e,
	 * após as remoções, reconstrói o arquivo de índice contendo apenas as chaves
	 * dos registros não removidos. Ao final, mostra os binários com `BinarioNaTela`.
	 */
	void func_9(char* arquivoBin, char* arquivoIndice, int n);

	/**
	 * @brief Funcionalidade [10]: Insere novos registros reaproveitando espaços removidos
	 * e atualiza o arquivo de índice em árvore-B com as chaves `codEstacao`.
	 * Utiliza a pilha de removidos do arquivo de dados e insere novas entradas na B-tree.
	 * @param arquivoBin Nome do arquivo binário de dados.
	 * @param arquivoIndice Nome do arquivo de índice árvore-B.
	 * @param n Quantidade de novos registros a serem inseridos.
	 * @return void
	 */
	void func_10(char* arquivoBin, char* arquivoIndice, int n);
#endif
