// playlist.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TITLE_LEN 100

struct Song {
    char title[TITLE_LEN];
    struct Song *next;
};

static struct Song *head = NULL;     // points to first song (or NULL)
static struct Song *current = NULL;  // points to currently playing song (or NULL)

/* Helper: create a new song node (returns NULL on malloc failure) */
static struct Song *create_song(const char *title) {
    struct Song *node = (struct Song *)malloc(sizeof(struct Song));
    if (!node) return NULL;
    /* safe copy with guaranteed null-termination */
    strncpy(node->title, title, TITLE_LEN - 1);
    node->title[TITLE_LEN - 1] = '\0';
    node->next = NULL;
    return node;
}

/* Add song to end of circular list */
void addSong(const char *title) {
    struct Song *newSong = create_song(title);
    if (!newSong) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return;
    }

    if (head == NULL) {
        head = newSong;
        newSong->next = head;   /* circular */
        current = head;
    } else {
        struct Song *temp = head;
        while (temp->next != head) {
            temp = temp->next;
        }
        temp->next = newSong;
        newSong->next = head;
    }
    printf("Added: \"%s\"\n", newSong->title);
}

/* Remove first occurrence of a song by title */
void removeSong(const char *title) {
    if (head == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    struct Song *temp = head;
    struct Song *prev = NULL;

    /* Search for the node */
    do {
        if (strcmp(temp->title, title) == 0) {
            /* If only one node in list */
            if (temp == head && head->next == head) {
                free(head);
                head = current = NULL;
                printf("Removed: \"%s\"\n", title);
                return;
            }

            /* Removing head in a list with >1 node */
            if (temp == head) {
                /* find last node so we can update its next to new head */
                struct Song *last = head;
                while (last->next != head)
                    last = last->next;

                head = head->next;
                last->next = head;

                /* update current if it pointed to removed node */
                if (current == temp)
                    current = head;
                free(temp);
                printf("Removed: \"%s\"\n", title);
                return;
            }

            /* Removing non-head node */
            if (prev != NULL) {
                prev->next = temp->next;
                if (current == temp)
                    current = temp->next;
                free(temp);
                printf("Removed: \"%s\"\n", title);
                return;
            }
        }
        prev = temp;
        temp = temp->next;
    } while (temp != head);

    printf("Song not found: \"%s\"\n", title);
}

/* Display all songs from head once (in order) */
void displayPlaylist(void) {
    if (head == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    struct Song *temp = head;
    printf("\n--- PLAYLIST ---\n");
    do {
        /* mark the currently playing song */
        if (temp == current)
            printf("-> %s  [CURRENT]\n", temp->title);
        else
            printf("   %s\n", temp->title);
        temp = temp->next;
    } while (temp != head);
    printf("----------------\n");
}

/* Play the current song (shows the title) */
void playCurrent(void) {
    if (current == NULL) {
        printf("No songs to play!\n");
        return;
    }
    printf("Now playing: %s\n", current->title);
}

/* Advance to next song and play it */
void playNext(void) {
    if (current == NULL) {
        printf("No songs to play!\n");
        return;
    }
    current = current->next;
    playCurrent();
}

/* Move to previous song and play it.
   We find the node whose next is current (that is the previous node). */
void playPrevious(void) {
    if (current == NULL) {
        printf("No songs to play!\n");
        return;
    }

    struct Song *temp = head;
    /* if only one node, prev is same as current */
    if (head->next == head) {
        playCurrent();
        return;
    }

    while (temp->next != current)
        temp = temp->next;

    current = temp;
    playCurrent();
}

/* Free entire playlist (used on program exit) */
void freePlaylist(void) {
    if (head == NULL) return;

    struct Song *temp = head->next;
    while (temp != head) {
        struct Song *next = temp->next;
        free(temp);
        temp = next;
    }
    free(head);
    head = current = NULL;
}

/* ----- MAIN (CLI) ----- */
int main(void) {
    int choice;
    char input[TITLE_LEN];

    while (1) {
        printf("\n===== MUSIC PLAYLIST (Circular Linked List) =====\n");
        printf("1. Add Song\n");
        printf("2. Remove Song\n");
        printf("3. Display Playlist\n");
        printf("4. Play Current\n");
        printf("5. Play Next\n");
        printf("6. Play Previous\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            /* invalid input - clear stdin and continue */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("Invalid input. Try again.\n");
            continue;
        }
        getchar(); /* consume newline */

        switch (choice) {
            case 1:
                printf("Enter song title: ");
                if (fgets(input, TITLE_LEN, stdin) == NULL) {
                    printf("Input error.\n");
                    break;
                }
                input[strcspn(input, "\n")] = '\0';
                if (strlen(input) == 0) {
                    printf("Empty title; not added.\n");
                    break;
                }
                addSong(input);
                break;

            case 2:
                printf("Enter song title to remove: ");
                if (fgets(input, TITLE_LEN, stdin) == NULL) {
                    printf("Input error.\n");
                    break;
                }
                input[strcspn(input, "\n")] = '\0';
                if (strlen(input) == 0) {
                    printf("Empty title.\n");
                    break;
                }
                removeSong(input);
                break;

            case 3:
                displayPlaylist();
                break;

            case 4:
                playCurrent();
                break;

            case 5:
                playNext();
                break;

            case 6:
                playPrevious();
                break;

            case 7:
                freePlaylist();
                printf("Exiting. Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice. Enter a number between 1 and 7.\n");
        }
    }

    return 0;
}
