#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição da estrutura de nó da lista circular duplamente encadeada
typedef struct Node {
    char artist[100];
    char song[100];
    struct Node *next;
    struct Node *prev;
} Node;

// Função para inserir um novo nó no final da lista
void insert(Node **head, Node **tail, char *artist, char *song) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->artist, artist);
    strcpy(newNode->song, song);

    if (*head == NULL) {
        *head = newNode;
        *tail = newNode;
        (*head)->next = *head;
        (*head)->prev = *head;
    } else {
        (*tail)->next = newNode;
        newNode->prev = *tail;
        newNode->next = *head;
        (*head)->prev = newNode;
        *tail = newNode;
    }
}

void insertNewItem(Node **head, Node **tail, char *artist, char *song) {
    Node *newNode = (Node *)malloc(sizeof(Node));

    if (newNode == NULL) {
        printf("Erro ao alocar memória para a nova música.\n");
        return;
    }

    // Copia o nome do artista e da música para o novo nó
    strncpy(newNode->artist, artist, sizeof(newNode->artist));
    newNode->artist[sizeof(newNode->artist) - 1] = '\0'; // Garante que a string esteja terminada
    strncpy(newNode->song, song, sizeof(newNode->song));
    newNode->song[sizeof(newNode->song) - 1] = '\0'; // Garante que a string esteja terminada

    if (*head == NULL) {
        *head = newNode;
        *tail = newNode;
        (*head)->next = *head;
        (*head)->prev = *head;
    } else {
        (*tail)->next = newNode;
        newNode->prev = *tail;
        newNode->next = *head;
        (*head)->prev = newNode;
        *tail = newNode;
    }

    // Abrir o arquivo para adicionar o novo registro
    FILE *arquivo = fopen("musicas.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fprintf(arquivo, "\n%s;%s", newNode->artist, newNode->song);
    fclose(arquivo);
}


// Função para imprimir a playlist pela ordem de cadastro
void printPlaylistByOrder(Node *head) {
    if (head == NULL) {
        printf("A playlist está vazia.\n");
        return;
    }
    Node *current = head;
    printf("Playlist pela ordem de cadastro:\n");
    do {
        printf("%s;%s\n", current->artist, current->song);
        current = current->next;
    } while (current != head);
}

// Função para encontrar uma música na playlist
void findSong(Node *head, char *targetSong) {
    if (head == NULL) {
        printf("A playlist está vazia.\n");
        return;
    }
    Node *current = head;
    int found = 0;
    do {
        if (strcmp(current->song, targetSong) == 0) {
            printf("Música encontrada: %s;%s\n", current->artist, current->song);
            found = 1;
            break;
        }
        current = current->next;
    } while (current != head);

    if (!found) {
        printf("Música não encontrada na playlist.\n");
    }
}

// Função para avançar para a próxima música
Node *nextSong(Node *current) {
    return current->next;
}

// Função para voltar para a música anterior
Node *previousSong(Node *current) {
    return current->prev;
}

// Function to remove a song from the playlist and the file
void removeSong(Node **head, Node **tail, char *songName) {
    if (*head == NULL) {
        printf("A playlist está vazia.\n");
        return;
    }
    Node *current = *head;
    int found = 0;

    do {
        // Use strncmp to compare the song names, considering spaces
        if (strncmp(current->song, songName, strlen(songName)) == 0) {
            // Found the song; adjust the next and prev pointers to remove it
            if (current == *head) {
                *head = current->next;
            }
            if (current == *tail) {
                *tail = current->prev;
            }
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // Remove the song from the file
            FILE *file = fopen("musicas.txt", "r");
            if (file == NULL) {
                printf("Erro ao abrir o arquivo.\n");
                exit(1);
            }

            FILE *tempFile = fopen("temp.txt", "w");
            if (tempFile == NULL) {
                printf("Erro ao criar o arquivo temporário.\n");
                exit(1);
            }

            char line[256];
            while (fgets(line, sizeof(line), file)) {
                char *artist = strtok(line, ";");
                char *song = strtok(NULL, "\n");
                // Use strncmp to compare the song names, considering spaces
                if (strncmp(song, songName, strlen(songName)) != 0) {
                    fprintf(tempFile, "%s;%s\n", artist, song);
                }
            }

            fclose(file);
            fclose(tempFile);

            remove("musicas.txt");
            rename("temp.txt", "musicas.txt");

            free(current); 

            printf("Música removida da playlist e do arquivo: %s\n", songName);
            found = 1;
            break;
        }
        current = current->next;
    } while (current != *head);

    if (!found) {
        printf("Música não encontrada na playlist.\n");
    }
}

// Função para imprimir a playlist pela ordem das músicas
void printPlaylistBySong(Node *head) {
    if (head == NULL) {
        printf("A playlist está vazia.\n");
        return;
    }

    // Contar o número de músicas na playlist
    int count = 0;
    Node *current = head;
    do {
        count++;
        current = current->next;
    } while (current != head);

    // Alocar um array para armazenar as músicas
    Node **songArray = (Node **)malloc(count * sizeof(Node *));
    if (songArray == NULL) {
        printf("Erro na alocação de memória.\n");
        exit(1);
    }

    // Copiar os nós da lista para o array
    current = head;
    for (int i = 0; i < count; i++) {
        songArray[i] = current;
        current = current->next;
    }

    // Ordenar o array com base no nome da música
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(songArray[i]->song, songArray[j]->song) > 0) {
                // Trocar os ponteiros no array para reorganizá-los
                Node *temp = songArray[i];
                songArray[i] = songArray[j];
                songArray[j] = temp;
            }
        }
    }

    // Imprimir as músicas na ordem das músicas
    printf("Playlist ordenada pelo nome das músicas:\n");
    for (int i = 0; i < count; i++) {
        printf("%s;%s\n", songArray[i]->artist, songArray[i]->song);
    }

    // Liberar a memória alocada para o array
    free(songArray);
}

int main() {
    FILE *file = fopen("musicas.txt", "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    Node *head = NULL;
    Node *tail = NULL;	

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *artist = strtok(line, ";");
        char *song = strtok(NULL, "\n");
        insert(&head, &tail, artist, song);
    }

    fclose(file);

    int choice;
    Node *current = head;
    do {
        printf("\nMenu:\n");
        printf("1. Exibir a playlist pela ordem de cadastro\n");
        printf("2. Exibir a playlist ordenada pelo nome das músicas\n");
        printf("3. Inserir uma nova música\n");
        printf("4. Remover uma música\n");
        printf("5. Buscar uma música\n");
        printf("6. Avançar para a próxima música\n");
        printf("7. Voltar para a música anterior\n");
        printf("8. Sair\n");
        printf("TOCANDO AGORA: %s\n", current->song);
        printf("Escolha uma opção: ");
        scanf("%d", &choice);
        fflush(stdin);

        switch (choice) {
            case 1:
                printPlaylistByOrder(head);
                break;
            case 2:
                printPlaylistBySong(head);
                break;
            case 3: {
               char artist[100], song[100];
          printf("Insira o nome do artista: ");
          scanf(" %[^\n]", artist);
          printf("Insira o nome da música: ");
          scanf(" %[^\n]", song); 
          insertNewItem(&head, &tail, artist, song);
                break;
            }
            case 4:{
                char songToRemove[100];
          printf("Insira o nome da música que deseja remover: ");
          scanf(" %[^\n]", songToRemove);
          fflush(stdin);
          removeSong(&head, &tail, songToRemove);
            break;
            }
            case 5: {
                char targetSong[100];
                printf("Insira o nome da música que você está procurando: ");
                scanf("%s", targetSong);
                fflush(stdin);
                findSong(head, targetSong);
                break;
            }
            case 6:
                current = nextSong(current);
                printf("Tocando agora: %s;%s\n", current->artist, current->song);
                break;
            case 7:
                current = previousSong(current);
                printf("Tocando agora: %s;%s\n", current->artist, current->song);
                break;
            case 8:
                printf("Saindo do programa.\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (choice != 8);


    return 0;
}
