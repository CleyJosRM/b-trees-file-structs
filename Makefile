# Cleyton José Rodrigues Macedo 16821725
# Guilherme Cavalcanti de Santana 15456556

# Variáveis
CC      := gcc
CFLAGS  := -Wall -Wextra -Iinclude -g -DPRINT_ERROS
# ADICIONE A SEGUINTE FLAG NA LINHA ACIMA PARA COMPILAR COM DEBUG: 
TARGET  := CPTM

# Pastas
SRC_DIR := src
OBJ_DIR := obj

# Arquivos
SRCS    := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/arvoreb/*.c) $(wildcard $(SRC_DIR)/arvore_avl/*.c) $(wildcard $(SRC_DIR)/core/*.c) $(wildcard $(SRC_DIR)/core/funcoes_core/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Regras
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run:
	./$(TARGET) 

.PHONY: all clean
