// I connected every possible Tetrimino to an integer:
// 
//     ....       ...#       ##..       .#..       #...       ###.       .#..
// 1 = ####   2 = ...#   3 = ##..   4 = ###.   5 = ##..   6 = .#..   7 = ##..
//     ....       ...#       ....       ....       #...       ....       .#..
//     ....       ...#       ....       ....       ....       ....       ....
//
//     .##.       #...        ##..        .#..        #...        ##..
// 8 = ##..   9 = ##..   10 = .##.   11 = ##..   12 = ###.   13 = #...   
//     ....       .#..        ....        #...        ....        #...
//     ....       ....        ....        ....        ....        ....
//
//      ###.        .#..        ..#.        #...        ###.        ##..
// 14 = ..#.   15 = .#..   16 = ###.   17 = #...   18 = #...   19 = .#..
//      ....        ##..        ....        ##..        ....        .#..
//      ....        ....        ....        ....        ....        ....

#include <stdio.h>
#include <stdlib.h>

typedef int bool;
#define true 1
#define false 0

// Declaring funtions
int tetrimino (int *arr);
int minMap (int n, int *tetriminos);
bool checkTet(int minSize, char arr[][minSize], int tetPos, int xPos, int yPos);
void placeTet(int minSize, char map[][minSize], int tetPiece, int xPos, int yPos, int number);
void removeTet(int minSize, char map[][minSize], int tetPos);
int getBlocks(char *file);
void identify(char *file, int *tetriminos);
void createMap(int mapSize, char map[][mapSize]);
void printMap(int mapSize, char map[][mapSize]);

//***************************************************************************************************************
//***************************************************************************************************************

int main(int argc, char *argv[]) {
    
    // Make sure you have a file as an argument

    if (argc != 2)
    {
        printf("Usage: ./fillit file\n");
        return 1;
    }
    
    char *file = argv[1];
    int blocks;
    
    // Count the number of blocks, to have a fixed Tetriminos size
    blocks = getBlocks(file);
    int tetriminos[blocks];
    int tetriminosXPos[blocks];
    int tetriminosYPos[blocks];
    
    // Pass the information about the single blocks to the identifier funciton
    identify(file, tetriminos);
    int tetriminosLength = sizeof(tetriminos)/sizeof(tetriminos[0]);
    
    // Create the map
    int mapSize = minMap(tetriminosLength, tetriminos);
    char map[mapSize][mapSize];
    
    // Declare/initialize variables
    int tetPos;
    int yPos;
    int xPos;
    int counter = 0;
    bool stop = false;
    
    //int checker = 0;
    
    // Brute-force
    completeRestart: ;
    createMap(mapSize, map);
    for(tetPos = 0; tetPos < tetriminosLength && !stop; tetPos++){
        newPiece: ;
        for(yPos = 0; yPos < mapSize && !stop; yPos++){
            newYRun: ;
            for(xPos = 0; xPos < mapSize && !stop; xPos++){
                newXRun: ;
                if(checkTet(mapSize, map, tetriminos[tetPos], xPos, yPos)){
                    placeTet(mapSize, map, tetriminos[tetPos], xPos, yPos, tetPos);
                    tetriminosXPos[tetPos] = xPos;
                    tetriminosYPos[tetPos] = yPos;
                    tetPos++;
                    counter ++;
                    if(tetPos < tetriminosLength){
                        goto newPiece;
                    }
                    if(tetPos == tetriminosLength){
                        stop = true;
                    }
                }
            }
        }
        
        // Only happnes, if all pieces are placed
        if(counter == tetriminosLength){
            break;
        }
        
        // Most likely to happen with piece number 1, or 2
        if(tetPos == 0){
            mapSize++;
            goto completeRestart;
        }
        
        // Every piece that gets to this point wasn't placed
        goPieceBack: ;
        removeTet(mapSize, map, (tetPos - 1));
        counter--;
        tetPos--;
        
        if(tetriminosXPos[tetPos] < (mapSize - 1)){
            xPos = (tetriminosXPos[tetPos] + 1);
            yPos = tetriminosYPos[tetPos];
            goto newXRun;
        }
        else if(tetriminosYPos[tetPos] < (mapSize - 1)){
            yPos = (tetriminosYPos[tetPos] + 1);
            goto newYRun;
        }
        else if(tetPos != 0){
            goto goPieceBack;
        }
        
        // Map is to small for current pieces, so enlarge it and restart
        else{
            mapSize++;
            goto completeRestart;
        }
        
    }
    
    printMap(mapSize, map);
    
    return 0;
}

//***************************************************************************************************************
//***************************************************************************************************************

// This functions return the theoretical minimum, if all pieces would fit neatly together
int minMap (int n, int *tetriminos){
    
    int counter = 0;
    int singlePieces = n*4;
    int minSize = 2;
    
    while((minSize*minSize) < singlePieces){
        minSize++;
    }
    
    // Exception if number 3 is the only block
    if(n == 1 && tetriminos[0] == 3){
        return 2;
    }
    int i;
    
    // For blocks number 1 and 2, to enlarge map if not large enough
    for(i = 0; i < n; i++){
        if(tetriminos[i] == 1 || tetriminos[i] == 2){
            counter++;
        }
    }
    if(minSize < 3){
        minSize = 3;
    }
    if(counter > 0 && minSize < 4){
        minSize = 4;
    }
    
    return minSize;
}

//***************************************************************************************************************
//***************************************************************************************************************

// Identifies what tetrinimo you have, and fill it inside of array
// The array contains the spacings between the single hashes
int tetrimino(int *arr){
    
    int sum = 0;
    int j;
    
    // Sum of spacings in the tetrinimo blocks
    
    for (j = 0; j < 3; j++){
        sum += arr[j];
    }
    
    // Identify which tetrimino you have, according to the spacings between the hashes
    switch(sum) {
        case 0: return 1; break;
        case 1: if(arr[0] == 1){
                    return 16;
                }
                else if(arr[1] == 1){
                    return 8; 
                }
                else if(arr[2] == 1){
                    return 18;
                }
                break;
        case 2: if(arr[1] == 2){
                    return 3;
                }
                else if(arr[0] == 2){
                    return 4;
                }
                else if(arr[2] == 2){
                    return 6;
                }
                break;
        case 3: if(arr[1] == 3){
                    return 10;
                }
                else if(arr[0] == 3){
                    return 12;
                }
                else if(arr[2] == 3){
                    return 14;
                }
                break;
        case 4: return 11; break;
        case 5: if(arr[2] == 2){
                    return 5;
                }
                else if(arr[0] == 2){
                    return 7;
                }
                else if(arr[0] == 0){
                    return 13;
                }
                else if(arr[0] == 3){
                    return 15;
                }
                break;
        case 6: if(arr[0] == 0){
                    return 19;
                }
                else if(arr[1] == 0){
                    return 9;
                }
                else if(arr[2] == 0){
                    return 17;
                }
                break;
        case 9: return 2; break;
        default: printf("ERROR"); return 42; break;
    }
    return 42;
} 

//***************************************************************************************************************
//***************************************************************************************************************

// Checks if the tetrimino would fit in the map, in the current position
bool checkTet(int minSize,char arr[][minSize], int tetPos, int xPos, int yPos){
    
    switch(tetPos){
        case 1: if((xPos + 3) > (minSize - 1) || yPos > (minSize - 1)){
                    return false;
                }
        
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos][xPos + 2] == '.' && arr[yPos][xPos + 3] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 2: if(xPos > (minSize - 1) || (yPos + 3) > (minSize - 1)){
                    return false;
                }
                
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 2][xPos] == '.' && arr[yPos + 3][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 3: if((xPos + 1) > (minSize - 1) || (yPos + 1) > (minSize - 1)){
                    return false;
                }
                
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 1][xPos + 1] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 4: if((xPos + 1) > (minSize - 1) || (yPos + 1) > (minSize - 1) || (xPos - 1) < 0){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos - 1] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 1][xPos + 1] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 5: if((xPos + 2) > (minSize - 1) || (yPos + 2) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 1][xPos + 1] == '.' && arr[yPos + 2][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 6: if((xPos + 2) > (minSize - 1) || (yPos + 1) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos][xPos + 2] == '.' && arr[yPos + 1][xPos + 1] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 7: if((xPos) > (minSize - 1) || (yPos + 2) > (minSize - 1) || (xPos - 1) < 0){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos - 1] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 2][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 8: if((xPos + 1) > (minSize - 1) || (yPos + 1) > (minSize - 1) || (xPos - 1) < 0){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos + 1][xPos - 1] == '.' && arr[yPos + 1][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 9: if((xPos + 1) > (minSize - 1) || (yPos + 2) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 1][xPos + 1] == '.' && arr[yPos + 2][xPos + 1] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 10:if((xPos + 2) > (minSize - 1) || (yPos + 1) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos + 1][xPos + 1] == '.' && arr[yPos + 1][xPos + 2] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 11:if((xPos) > (minSize - 1) || (yPos + 2) > (minSize - 1) || (xPos - 1) < 0){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 1][xPos - 1] == '.' && arr[yPos + 2][xPos - 1] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 12:if((xPos + 2) > (minSize - 1) || (yPos + 1) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 1][xPos + 1] == '.' && arr[yPos + 1][xPos + 2] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 13:if((xPos + 1) > (minSize - 1) || (yPos + 2) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 2][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 14:if((xPos + 2) > (minSize - 1) || (yPos + 1) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos][xPos + 2] == '.' && arr[yPos + 1][xPos + 2] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 15:if((xPos) > (minSize - 1) || (yPos + 2) > (minSize - 1) || (xPos - 1) < 0){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 2][xPos - 1] == '.' && arr[yPos + 2][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 16:if((xPos) > (minSize - 1) || (yPos + 1) > (minSize - 1) || (xPos - 2) < 0){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos - 2] == '.' && arr[yPos + 1][xPos - 1] == '.' && arr[yPos + 1][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 17:if((xPos + 1) > (minSize - 1) || (yPos + 2) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos + 1][xPos] == '.' && arr[yPos + 2][xPos] == '.' && arr[yPos + 2][xPos + 1] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 18:if((xPos + 2) > (minSize - 1) || (yPos + 1) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos][xPos + 2] == '.' && arr[yPos + 1][xPos] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
        case 19:if((xPos + 1) > (minSize - 1) || (yPos + 2) > (minSize - 1)){
                    return false;
                }
                if(arr[yPos][xPos] == '.' && arr[yPos][xPos + 1] == '.' && arr[yPos + 1][xPos + 1] == '.' && arr[yPos+ 2][xPos + 1] == '.'){
                    return true;
                }
                else{
                    return false;
                }
                break;
    }
    
    return false;
    
}

//***************************************************************************************************************
//***************************************************************************************************************

// Places the tetriminos with their correct letter in the map
void placeTet(int minSize, char map[][minSize], int tetPiece, int xPos, int yPos, int number){
    switch(tetPiece){
                            case 1: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos][xPos + 2] = number + 'A'; 
                                    map[yPos][xPos + 3] = number + 'A';
                                    break;
                            case 2: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 2][xPos] = number + 'A'; 
                                    map[yPos + 3][xPos] = number + 'A';
                                    break;
                            case 3: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A';
                                    break;
                            case 4: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos - 1] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A';
                                    break;
                            case 5: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A'; 
                                    map[yPos + 2][xPos] = number + 'A';
                                    break;
                            case 6: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos][xPos + 2] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A';
                                    break;
                            case 7: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos - 1] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 2][xPos] = number + 'A';
                                    break;
                            case 8: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos + 1][xPos - 1] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A';
                                    break;
                            case 9: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A'; 
                                    map[yPos + 2][xPos + 1] = number + 'A';
                                    break;
                            case 10: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A'; 
                                    map[yPos + 1][xPos + 2] = number + 'A';
                                    break;
                            case 11: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos - 1] = number + 'A'; 
                                    map[yPos + 2][xPos - 1] = number + 'A';
                                    break;;
                            case 12: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A'; 
                                    map[yPos + 1][xPos + 2] = number + 'A';
                                    break;
                            case 13: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 2][xPos] = number + 'A';
                                    break;
                            case 14: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos][xPos + 2] = number + 'A'; 
                                    map[yPos + 1][xPos + 2] = number + 'A';
                                    break;
                            case 15: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 2][xPos - 1] = number + 'A'; 
                                    map[yPos + 2][xPos] = number + 'A';
                                    break;
                            case 16: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos - 2] = number + 'A'; 
                                    map[yPos + 1][xPos - 1] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A';
                                    break;
                            case 17: map[yPos][xPos] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A'; 
                                    map[yPos + 2][xPos] = number + 'A'; 
                                    map[yPos + 2][xPos + 1] = number + 'A';
                                    break;
                            case 18: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos][xPos + 2] = number + 'A'; 
                                    map[yPos + 1][xPos] = number + 'A';
                                    break;
                            case 19: map[yPos][xPos] = number + 'A'; 
                                    map[yPos][xPos + 1] = number + 'A'; 
                                    map[yPos + 1][xPos + 1] = number + 'A'; 
                                    map[yPos + 2][xPos + 1] = number + 'A';
                                    break;
                        }
    
}

//***************************************************************************************************************
//***************************************************************************************************************

// Deletes one tetrimino from the map
void removeTet(int minSize, char map[][minSize], int tetPos){
    
    char r = tetPos + 'A';
    int i;
    int j;
    for(i = 0; i < minSize; i++){
        for(j = 0; j < minSize; j++){
            if(map[i][j] == r){
                map[i][j] = '.';
            }
        }
    }
}

//***************************************************************************************************************
//***************************************************************************************************************

// Returns the number of blocks/tetriminos you have
int getBlocks(char *file){
    
    char c;
    int line = 0;
    int pos = 0;
    int blocks;
    
    FILE *inptr = fopen(file, "rt");
    
    // Count the amount of blocks/tetriminos
    while((c=fgetc(inptr))!=EOF){
        
        if(c == '.' || c == '#'){
            pos++;
            if(pos == 4){
                pos = 0;
                line++;
            }
        }
    }
    
    blocks = (line/4);
    fclose(inptr);
    
    return blocks;
}

//***************************************************************************************************************
//***************************************************************************************************************

// Counts the spaces, to pass the information to the "tetrimino" function
void identify(char *file, int *tetriminos){
    
    char c;
    int pos = 0;
    int space = 0;
    int spacings[3];
    int number = 0;
    int a = 0;
    int b = 0;
    int dotCount = 0;
    
    FILE *inptr_new = fopen(file, "rt");
    
    // Pass the information about the single blocks to the identifier funciton
    while((c=fgetc(inptr_new))!=EOF){
        
        if(b == 2 && c != '\n'){
            printf("Invalid file! ");
            abort();
        }
        if(b == 2){
            b = 0;
        }
        if(b == 1){
            b++;
        }
        if(c == '#'){
            if(a != 0){
                spacings[a - 1] = space;
            }
            a++;
            space = 0;
            pos++;
        }
        if(c == '.'){
            space++;
            pos++;
            dotCount++;
        }
        if(c != '#' && c != '.' && c != '\n'){
            printf("Invalid file! ");
            abort();
        }
        if(c != '#' && c != '.'){
            continue;
        }
            
        if(pos != 0 && pos % 16 == 0){
            if(a != 4 || dotCount != 12){
                printf("Invalid file! ");
                abort();
            }
            tetriminos[number] = tetrimino(spacings);
            number++;
            a = 0;
            dotCount = 0;
            b = 1;
        }
    }
    
    fclose(inptr_new);
}

//***************************************************************************************************************
//***************************************************************************************************************

// Create map out of dots
void createMap(int mapSize, char map[][mapSize]){
    
    int m;
    for(m = 0; m < mapSize; m++){
        int n;
        for(n = 0; n < mapSize; n++){
            map[m][n] = '.';
        }
    }
}

//***************************************************************************************************************
//***************************************************************************************************************

// Prints out the current map
void printMap(int mapSize, char map[][mapSize]){
    
    int m;
    for(m = 0; m < mapSize; m++){
        int n;
        for(n = 0; n < mapSize; n++){
            printf("%c", map[m][n]);
        }
        printf("\n");
    }
}