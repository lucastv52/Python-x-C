#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_CHAVE 10
#define MAX_N 1000000
#define PASSO 50000



typedef struct Item {
    char *key;
    int value;
    struct Item *next;
} Item;

typedef struct Dictionary {
    int size;
    int count;
    Item **table;
} Dictionary;


unsigned int hashCode(const char *key, int table_size) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % table_size;
}


Dictionary *createDictionary(int size) {
    Dictionary *dict = malloc(sizeof(Dictionary));
    if (!dict) {
        perror("Falha ao alocar dicionário");
        exit(EXIT_FAILURE);
    }
    dict->size = size;
    dict->table = calloc(size, sizeof(Item *));
    if (!dict->table) {
        perror("Falha ao alocar tabela hash");
        free(dict);
        exit(EXIT_FAILURE);
    }
    return dict;
}

void dictInsert(Dictionary *dict, const char *key, int value) {
    unsigned int index = hashCode(key, dict->size);
    Item *current = dict->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    Item *newItem = malloc(sizeof(Item));
    newItem->key = strdup(key);
    newItem->value = value;
    newItem->next = dict->table[index];
    dict->table[index] = newItem;
}

int dictSearch(Dictionary *dict, const char *key) {
    unsigned int index = hashCode(key, dict->size);
    Item *current = dict->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;
}

int dictRemove(Dictionary *dict, const char *key) {
    unsigned int index = hashCode(key, dict->size);
    Item *current = dict->table[index], *prev = NULL;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev) prev->next = current->next;
            else dict->table[index] = current->next;
            free(current->key);
            free(current);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

void freeDictionary(Dictionary *dict) {
    for (int i = 0; i < dict->size; i++) {
        Item *current = dict->table[i];
        while (current != NULL) {
            Item *temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(dict->table);
    free(dict);
}



void gerarChaveAleatoria(char *destino, int tamanho) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < tamanho - 1; i++) {
        int indice = rand() % (int)(sizeof(charset) - 1);
        destino[i] = charset[indice];
    }
    destino[tamanho - 1] = '\0';
}

void printDictionary(Dictionary *dict) {
    printf("Dicionário:\n");
    for (int i = 0; i < dict->size; i++) {
        Item *current = dict->table[i];
        if (current != NULL) {            
            while (current != NULL) {
                printf("  [%s] => %d\n", current->key, current->value);
                current = current->next;
            }
        }
    }
}

int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void printSortedDictionary(Dictionary *dict) {
    if (!dict || dict->count == 0) {
        printf("Dicionário está vazio ou é nulo.\n");
        return;
    }

 
    char **keys = (char **)malloc(dict->count * sizeof(char *));
    if (!keys) {
        perror("Falha ao alocar array de chaves para ordenação");
        return;
    }

    int k_idx = 0;
    for (int i = 0; i < dict->size; i++) {
        Item *current = dict->table[i];
        while (current != NULL) {
            if (k_idx < dict->count) { 
                keys[k_idx++] = current->key;
            }
            current = current->next;
        }
    }

    qsort(keys, dict->count, sizeof(char *), compareStrings);
    printf("Dicionário (Ordenado Alfabeticamente):\n");
    for (int i = 0; i < dict->count; i++) {
        int value = dictSearch(dict, keys[i]); 
        printf("  '%s': %d\n", keys[i], value);
    }

    free(keys); 
}



int main() {
    srand(time(NULL));

    for (int N = 0; N <= MAX_N; N += PASSO) {
        if (N == 0) continue;

        printf("\n------ TESTE COM %d ELEMENTOS ------\n", N);

        Dictionary *dict = createDictionary((int)(N / 0.5) + 1); // Fator de carga 0.5
        char **chaves = malloc(sizeof(char *) * N);

    

        // Inserção
        clock_t inicio = clock();
        for (int i = 0; i < N; i++) {
            char chave[TAM_CHAVE];
            gerarChaveAleatoria(chave, TAM_CHAVE);
            chaves[i] = strdup(chave);
            dictInsert(dict, chaves[i], rand() % 100000);
        }
        clock_t fim = clock();
        double tempoInsercao = (double)(fim - inicio) / CLOCKS_PER_SEC;
        printf("Tempo de inserção: %.6f s\n", tempoInsercao);

        // Busca
        inicio = clock();
        for (int i = 0; i < 0.9 * N; i++) {
            dictSearch(dict, chaves[i]);
        }
        for (int i = 0; i < 0.1 * N; i++) {
            char fake[TAM_CHAVE];
            gerarChaveAleatoria(fake, TAM_CHAVE);
            dictSearch(dict, fake);
        }
        fim = clock();
        double tempoBusca = (double)(fim - inicio) / CLOCKS_PER_SEC;
        printf("Tempo de busca: %.6f s\n", tempoBusca);

        // Remoção
        inicio = clock();
        for (int i = N - 1; i >= 0; i--) {
            dictRemove(dict, chaves[i]);

        }

        fim = clock();
        double tempoRemocao = (double)(fim - inicio) / CLOCKS_PER_SEC;
        printf("Tempo de remoção: %.6f s\n", tempoRemocao);

        free(chaves);
        freeDictionary(dict);
    }

    return 0;
}
