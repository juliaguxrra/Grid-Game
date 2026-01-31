#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DICTIONARY_FILE "dictionary.txt" //file with valid words
#define BOARD_SIZE 6 //number of rows on he game board
#define MAX_TRIES 6 //max number of tries allowed
#define WORD_SIZE 5 //length of word user will guess
#define WORD_LIST 5757 //number of words in dicionary

//creates ncurses window 
WINDOW* create_window(int height, int width, int start_y, int start_x) {
    WINDOW* temp = newwin(height, width, start_y, start_x); // creates a window
    box(temp, 0, 0); // adds border
    wrefresh(temp); //refreshes window to display new changes 
    return temp;
}

// Reads through dictionary.txt and creates an array of valid words
char **dictionary(){
   FILE *file = fopen("dictionary.txt", "r");
    char **words = malloc(sizeof(char*) * WORD_LIST); // Allocating memory for each word
    int count = 0;
    char buffer[10];  // Temporary buffer for reading words

    while (count < WORD_LIST && fgets(buffer, sizeof(buffer), file)) {
        words[count] = malloc(sizeof(buffer));
        buffer[strcspn(buffer, "\n")] = '\0';
        strcpy(words[count], buffer);  // Allocate and copy word
        count++;
    }
    fclose(file);
    return words;
}

//frees memory allocated for the dictionary array
void free_dictionary(char **dictionary){
    for(int i = 0; i < WORD_LIST; i++){
        if(dictionary[i]){
            free(dictionary[i]);
        }
    }
    free(dictionary);
}

// Returns random word from dictionary
char *ran_word(char **words) {

    // Generate a random number to serve as index between 0 and 5757
    int random_number = (rand() % WORD_LIST);

    return(words[random_number]);
}
//displays game instructions for player
void instructions(WINDOW *game_win){
    mvprintw(1, 55, "WELCOME TO WORDLE!");
    mvprintw(2, 55, "You get 6 chances to guess a 5 letter word.");
    wrefresh(game_win);
}

// Generates a 2d array for guesses
char **create_board() {
    char **board = malloc(sizeof(char *) * BOARD_SIZE);// allocating memory for board
    for(int i = 0; i < BOARD_SIZE; i++) {
        board[i] = malloc(sizeof(char) * (WORD_SIZE + 1));// allocating memory for rows
        memset(board[i], ' ', WORD_SIZE);
        board[i][WORD_SIZE] = '\0'; 
    }
    return board;
}

// Frees the 2d array
void free_board(char **board){
    for(int i = 0; i < BOARD_SIZE; i++){
        free(board[i]);
    }
    free(board);
}

// Checks if inputted guess is valid
int is_valid_word(char *word, char **dictionary){
    if (strlen(word)!=WORD_SIZE){ // Checks if it is 5 letters long
        return 0; 
    }
    for(int i=0; i<WORD_LIST; i++){
        if (strcmp(word, dictionary[i])==0){ // Checks if guess is in dictionary
            return 1;  
        }
    }
    return 0; 
}

//printing the game board and colors letters based on correctness
void print_window(WINDOW *window, char **board, char* secret_word){
    werase(window);
    box(window, 0, 0); // draws box around board
    
    for(int i = 0; i < MAX_TRIES; i++){
        wmove(window, i+1,1);

        // Prints all the letters with the correct color backgrounds
        for(int x = 0; x < WORD_SIZE; x++){
            if(board[i][x]==secret_word[x]){
            wattron(window, COLOR_PAIR(1));
                wprintw(window, " %c ", board[i][x]);
                wattroff(window, COLOR_PAIR(1));
            } 
            else if(strchr(secret_word, board[i][x])){
                wattron(window, COLOR_PAIR(2));
                wprintw(window, " %c ", board[i][x]);
                wattroff(window, COLOR_PAIR(2));
            } 
            else{
                wprintw(window, " %c ", board[i][x]);
            }
        }
    }
    wrefresh(window);
}

// Adds the guessed word to board
void add_guess(char **board, char* guess, int guesses, char* secret_word, WINDOW *game_win){
    for(int i = 0; i < WORD_SIZE; i++){
        board[guesses][i] = guess[i];
    }
    print_window(game_win, board, secret_word);
}
//adds the player's guess to the board and the board is updated
void game_loop(char **dictionary, char **board, WINDOW *game_win){ 
    char *secret_word = malloc(sizeof(char) * WORD_SIZE + 1);
    strcpy(secret_word, ran_word(dictionary)); // Creates the random secret word
    int guesses= 0;
    int pos = 0;
    char guess[WORD_SIZE + 1]= {0};
    char cheat_mode[] = "cheat";
    print_window(game_win, board, secret_word);
    while (guesses < MAX_TRIES) {
        int ch = getch();

        // Handle input characters
        if (isalpha(ch) && pos < WORD_SIZE) { // Checks if the character typed is a letter and if no more than 4 letters have already been typed
            guess[pos++] = tolower(ch);
            mvwprintw(game_win, guesses * 2 + 1, (pos - 1) * 4 + 3, "%c", toupper(ch)); // Prints typed letter
            wrefresh(game_win);
        } else if (ch == 127 || ch == KEY_BACKSPACE) { // Handles backspace
            if (pos > 0) {
                pos--;
                mvwprintw(game_win, guesses * 2 + 1, pos * 4 + 3, " "); // Removes letter if backspace is typed
                wrefresh(game_win);
            }
        } else if (ch == '\n' && pos == WORD_SIZE) { // Handles Enter key
            guess[WORD_SIZE] = '\0';

            if(strcmp(guess, cheat_mode) == 0){ // Checks if "cheat" is guessed to print the secret word
                mvprintw(6, 55, "Secret Word is: %s", secret_word);
                wrefresh(game_win);
            }

            if(is_valid_word(guess, dictionary)){ // Checks if the word is valid and if so adds it to the board and window
                add_guess(board, guess, guesses, secret_word, game_win);
                guesses++;
                pos = 0;
            }

            if (strcmp(guess, secret_word) == 0) { // User wins game
                mvprintw(8, 55, "You Win!");
                wrefresh(game_win);
                break;
            }
            else if (guesses == MAX_TRIES) { // Loses game
                mvprintw(8, 55, "You Lose! Secret Word was: %s", secret_word);
                wrefresh(game_win);
            }
        }
    }

    free(secret_word);
    getch();
}


int main() {
    srand(time(NULL));
    initscr();
    noecho();
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_GREEN); // Creates colored pairs for correct and almost correct guessed letters
    init_pair(2,COLOR_BLUE,COLOR_YELLOW);
    char **board = create_board();
    WINDOW *game_win = create_window(15, 20, 0, 30); // Creates the board of these sizes
    char **words = dictionary();
    instructions(game_win);
    game_loop(words, board, game_win);
    delwin(game_win); // Deletes the window
    free_dictionary(words);
    free_board(board);
    endwin();
    return 0;
}