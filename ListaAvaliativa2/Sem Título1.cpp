#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defini��o da estrutura de n� da lista circular duplamente encadeada
typedef struct Node {
    char artist[100];
    char song[100];
    struct Node *next;
    struct Node *prev;
} Node;

// Fun��o para inserir um novo n� no final da lista
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
        printf("Erro ao alocar mem�ria para a nova m�sica.\n");
        return;
    }

    // Copia o nome do artista e da m�sica para o novo n�
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


// Fun��o para imprimir a playlist pela ordem de cadastro
void printPlaylistByOrder(Node *head) {
    if (head == NULL) {
        printf("A playlist est� vazia.\n");
        return;
    }
    Node *current = head;
    printf("Playlist pela ordem de cadastro:\n");
    do {
        printf("%s;%s\n", current->artist, current->song);
        current = current->next;
    } while (current != head);
}

// Fun��o para encontrar uma m�sica na playlist
void findSong(Node *head, char *targetSong) {
    if (head == NULL) {
        printf("A playlist est� vazia.\n");
        return;
    }
    Node *current = head;
    int found = 0;
    do {
        if (strcmp(current->song, targetSong) == 0) {
            printf("M�sica encontrada: %s;%s\n", current->artist, current->song);
            found = 1;
            break;
        }
        current = current->next;
    } while (current != head);

    if (!found) {
        printf("M�sica n�o encontrada na playlist.\n");
    }
}

// Fun��o para avan�ar para a pr�xima m�sica
Node *nextSong(Node *current) {
    return current->next;
}

// Fun��o para voltar para a m�sica anterior
Node *previousSong(Node *current) {
    return current->prev;
}

// Function to remove a song from the playlist and the file
void removeSong(Node **head, Node **tail, char *songName) {
    if (*head == NULL) {
        printf("A playlist est� vazia.\n");
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
                printf("Erro ao criar o arquivo tempor�rio.\n");
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

            printf("M�sica removida da playlist e do arquivo: %s\n", songName);
            found = 1;
            break;
        }
        current = current->next;
    } while (current != *head);

    if (!found) {
        printf("M�sica n�o encontrada na playlist.\n");
    }
}

// Fun��o para imprimir a playlist pela ordem das m�sicas
void printPlaylistBySong(Node *head) {
    if (head == NULL) {
        printf("A playlist est� vazia.\n");
        return;
    }

    // Contar o n�mero de m�sicas na playlist
    int count = 0;
    Node *current = head;
    do {
        count++;
        current = current->next;
    } while (current != head);

    // Alocar um array para armazenar as m�sicas
    Node **songArray = (Node **)malloc(count * sizeof(Node *));
    if (songArray == NULL) {
        printf("Erro na aloca��o de mem�ria.\n");
        exit(1);
    }

    // Copiar os n�s da lista para o array
    current = head;
    for (int i = 0; i < count; i++) {
        songArray[i] = current;
        current = current->next;
    }

    // Ordenar o array com base no nome da m�sica
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(songArray[i]->song, songArray[j]->song) > 0) {
                // Trocar os ponteiros no array para reorganiz�-los
                Node *temp = songArray[i];
                songArray[i] = songArray[j];
                songArray[j] = temp;
            }
        }
    }

    // Imprimir as m�sicas na ordem das m�sicas
    printf("Playlist ordenada pelo nome das m�sicas:\n");
    for (int i = 0; i < count; i++) {
        printf("%s;%s\n", songArray[i]->artist, songArray[i]->song);
    }

    // Liberar a mem�ria alocada para o array
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
        printf("2. Exibir a playlist ordenada pelo nome das m�sicas\n");
        printf("3. Inserir uma nova m�sica\n");
        printf("4. Remover uma m�sica\n");
        printf("5. Buscar uma m�sica\n");
        printf("6. Avan�ar para a pr�xima m�sica\n");
        printf("7. Voltar para a m�sica anterior\n");
        printf("8. Sair\n");
        printf("TOCANDO AGORA: %s\n", current->song);
        printf("Escolha uma op��o: ");
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
          printf("Insira o nome da m�sica: ");
          scanf(" %[^\n]", song); 
          insertNewItem(&head, &tail, artist, song);
                break;
            }
            case 4:{
                char songToRemove[100];
          printf("Insira o nome da m�sica que deseja remover: ");
          scanf(" %[^\n]", songToRemove);
          fflush(stdin);
          removeSong(&head, &tail, songToRemove);
            break;
            }
            case 5: {
                char targetSong[100];
                printf("Insira o nome da m�sica que voc� est� procurando: ");
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
                printf("Op��o inv�lida. Tente novamente.\n");
        }
    } while (choice != 8);


    return 0;
}
