/**
 * ============================================================================
 * @file        indice.h
 * @brief       Interface pública de Índice implementado como Árvore-B

 * @author      Cleyton Jose Rodrigues Macedo 16821725
 * @author      Guilherme Cavalcanti de Santana 15456556
 *
 * @details     Fornece operações de busca, inserção e remoção em uma estrutura
 *              de índice B-Tree persistida em arquivo. A árvore mantém dados
 *              ordenados com garantias de balanceamento.
 *
 * @note        O arquivo de índice usa um formato binário específico com
 *              cabeçalho de controle. O status de consistência é automaticamente
 *              gerenciado durante operações de escrita.
 * ============================================================================
 */

#ifndef ARVOREB_H
	#define ARVOREB_H

	#include <stdio.h>
	#include <stdbool.h>

	//                             FUNÇÕES PÚBLICAS                               

	/**
	 * @brief       Abre um arquivo de índice B-Tree para leitura ou escrita
	 *
	 * @param[in]   nomeIndice  Nome/caminho do arquivo de índice
	 * @param[in]   modo        Modo de abertura ("rb", "rb+", "wb", "wb+")
	 *
	 * @return      Ponteiro FILE* válido se bem-sucedido
	 * @return      NULL se ocorrer erro (arquivo não encontrado, modo inválido,
	 *              inconsistência detectada, etc.)
	 *
	 * @remarks     - O arquivo é automaticamente marcado como inconsistente
	 *                durante operações de escrita
	 *              - Se o arquivo estiver inconsistente na abertura, a função
	 *                retorna NULL e não abre o arquivo
	 *              - O chamador é responsável por chamar fechar_indice() após uso
	 */
	FILE *abrir_indice(char *nomeIndice, char *modo);

	/**
	 * @brief       Fecha um arquivo de índice B-Tree com estado consistente
	 *
	 * @param[in]   indice  Ponteiro FILE* retornado por abrir_indice()
	 * @param[in]   modo    Mesmo modo usado na abertura
	 *
	 * @return      true    Se o fechamento foi bem-sucedido
	 * @return      false   Se ocorreu erro na operação de fechamento
	 *
	 * @remarks     - Marca o arquivo como CONSISTENTE antes de fechar se
	 *                ele foi aberto em modo de escrita
	 *              - Se indice for NULL, retorna true
	 *              - Modo de leitura ("rb") não marca como consistente
	 */
	bool fechar_indice(FILE *indice, char *modo);

	/**
	 * @brief       Inicializa um arquivo de índice B-Tree novo
	 *
	 * @param[in]   arquivo     Ponteiro FILE* aberto em modo escrita ("wb+")
	 *                          e posicionado no início
	 *
	 * @return      void
	 *
	 * @remarks     - Escreve o cabeçalho inicial no arquivo (17 bytes)
	 *              - Define RRN raiz como -1 (árvore vazia)
	 *              - Inicializa contadores de nós e RRN sequencial
	 *              - Marca arquivo como inconsistente
	 *              - Deve ser chamado apenas uma vez na criação do índice
	 */
	void criar_indice(FILE *arquivo);

	/**
	 * @brief       Busca uma entrada na árvore-B por chave
	 *
	 * @param[in]   indice          Ponteiro FILE* do índice aberto
	 * @param[in]   chaveBusca      Chave inteira a ser procurada (codEstacao)
	 *
	 * @return      Byte offset (BO) do registro de dados se encontrado
	 * @return      -1              Se a chave não existe na árvore
	 *
	 * @remarks     - Não modifica o estado do arquivo de índice
	 *              - O valor retornado pode ser usado para localizar o registro
	 *                no arquivo de dados principal
	 */
	int buscar_entrada(FILE *indice, int chaveBusca);

	/**
	 * @brief       Insere uma nova entrada na árvore-B
	 *
	 * @param[in]   indice      Ponteiro FILE* do índice aberto em modo escrita
	 * @param[in]   chave       Chave inteira da entrada (deve ser única)
	 * @param[in]   BOdados     Byte offset do registro correspondente no arquivo
	 *                          de dados
	 *
	 * @return      void
	 *
	 * @remarks     - Mantém a ordenação interna da árvore B
	 *              - Realiza rebalanceamento automático se necessário
	 *              - O arquivo é marcado como inconsistente durante a operação
	 *              - O rebalanceamento pode aumentar a altura da árvore
	 */
	void inserir_entrada(FILE *indice, int chave, int BOdados);

	/**
	 * @brief       Remove uma entrada da árvore-B por chave
	 *
	 * @param[in]   indice  Ponteiro FILE* do índice aberto em modo escrita
	 * @param[in]   chave   Chave inteira a ser removida (codEstacao)
	 *
	 * @return      void
	 *
	 * @remarks     - Se a chave não existe, a operação é sem efeito
	 *              - Realiza rebalanceamento automático mantendo a B-Tree válida
	 *              - Pode reduzir a altura da árvore
	 *              - Páginas removidas são adicionadas a uma pilha de livres
	 */
	void remover_entrada(FILE *indice, int chave);

#endif /* ARVOREB_H */