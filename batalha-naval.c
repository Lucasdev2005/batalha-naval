/*
 * Batalha Naval (Player vs CPU)
 * Arquivo único em C - compilável com gcc
 * Uso: gcc -o batalhanaval BatalhaNaval.c && ./batalhanaval
 * Entrada: coordenadas no formato A1..J10 (letras A-J ou a-j) e números 1-10
 * Autor: gerado pelo ChatGPT
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define N 10

typedef enum { WATER = 0, SHIP = 1, HIT = 2, MISS = 3 } Cell;

const int ship_sizes[] = {5, 4, 3, 3, 2};
const char *ship_names[] = {"Porta-aviões", "Encouraçado", "Cruzador", "Submarino", "Destroyer"};
const int SHIP_COUNT = sizeof(ship_sizes) / sizeof(ship_sizes[0]);

void init_board(Cell board[N][N]){
    for(int i=0;i<N;i++) for(int j=0;j<N;j++) board[i][j]=WATER;
}

void print_boards(Cell player[N][N], Cell view_of_cpu[N][N]){
    // Print player's own board at left and view of CPU (fog of war) at right
    printf("\n    Seu tabuleiro\t\t\t    Tabuleiro do inimigo (apenas ataques)\n");
    printf("   ");
    for(int c=0;c<N;c++) printf(" %2d", c+1);
    printf("\t\t   ");
    for(int c=0;c<N;c++) printf(" %2d", c+1);
    printf("\n");

    for(int r=0;r<N;r++){
        printf(" %c ", 'A'+r);
        for(int c=0;c<N;c++){
            char ch;
            switch(player[r][c]){
                case WATER: ch='~'; break;
                case SHIP: ch='S'; break;
                case HIT: ch='X'; break;
                case MISS: ch='o'; break;
                default: ch='?';
            }
            printf("  %c", ch);
        }
        printf("\t\t %c ", 'A'+r);
        for(int c=0;c<N;c++){
            char ch;
            switch(view_of_cpu[r][c]){
                case WATER: ch='~'; break; // unknown still
                case SHIP: ch='S'; break;   // we won't show CPU ships here normally
                case HIT: ch='X'; break;
                case MISS: ch='o'; break;
                default: ch='?';
            }
            if(view_of_cpu[r][c]==SHIP) ch='~'; // hide remaining CPU ships
            printf("  %c", ch);
        }
        printf("\n");
    }
}

int can_place(Cell board[N][N], int r, int c, int size, int horiz){
    if(horiz){
        if(c+size> N) return 0;
        for(int i=0;i<size;i++) if(board[r][c+i]!=WATER) return 0;
    } else {
        if(r+size> N) return 0;
        for(int i=0;i<size;i++) if(board[r+i][c]!=WATER) return 0;
    }
    return 1;
}

void place_ship(Cell board[N][N], int r, int c, int size, int horiz){
    if(horiz){
        for(int i=0;i<size;i++) board[r][c+i]=SHIP;
    } else {
        for(int i=0;i<size;i++) board[r+i][c]=SHIP;
    }
}

void place_ships_random(Cell board[N][N]){
    for(int s=0;s<SHIP_COUNT;s++){
        int size = ship_sizes[s];
        int placed = 0;
        while(!placed){
            int horiz = rand()%2;
            int r = rand()%N;
            int c = rand()%N;
            if(can_place(board,r,c,size,horiz)){
                place_ship(board,r,c,size,horiz);
                placed = 1;
            }
        }
    }
}

void player_place_manual(Cell board[N][N]){
    char line[32];
    printf("\nColoque suas embarcações. Você pode digitar 'auto' para posicionar aleatoriamente.\n");
    for(int s=0;s<SHIP_COUNT;s++){
        int size = ship_sizes[s];
        int placed = 0;
        while(!placed){
            printf("\nPosicionar %s (tamanho %d). Ex.: A1 H (H para horizontal, V para vertical) ou 'auto'\n", ship_names[s], size);
            printf("> ");
            if(!fgets(line,sizeof(line),stdin)) exit(1);
            // trim
            for(char *p=line; *p; ++p) if(*p=='\n') *p='\0';
            if(strlen(line)==0) continue;
            if(strcmp(line,"auto")==0){
                place_ships_random(board);
                return;
            }
            // parse like A1 H
            char colc; int row; char dir;
            if(sscanf(line, " %c%d %c", &colc, &row, &dir) >=2){
                colc = toupper(colc);
                dir = toupper(dir);
                int r = colc - 'A';
                int c = row - 1;
                int horiz = (dir=='H');
                if(r<0||r>=N||c<0||c>=N){ printf("Coordenada inválida.\n"); continue; }
                if(dir!='H' && dir!='V') { printf("Direção inválida. Use H ou V.\n"); continue; }
                if(!can_place(board,r,c,size,horiz)){ printf("Não cabe aí (sobreposição ou limite). Tente outra posição.\n"); continue; }
                place_ship(board,r,c,size,horiz);
                placed = 1;
                print_boards(board, board); // show placements (right board same as left just to visualize)
            } else {
                printf("Entrada inválida. Formato: A1 H ou 'auto'\n");
            }
        }
    }
}

int attack(Cell target[N][N], Cell view[N][N], int r, int c){
    if(view[r][c]==HIT || view[r][c]==MISS) return -1; // already shot
    if(target[r][c]==SHIP){
        target[r][c]=HIT; view[r][c]=HIT; return 1; // hit
    } else {
        if(target[r][c]==WATER) target[r][c]=MISS; view[r][c]=MISS; return 0; // miss
    }
}

int ships_remaining(Cell board[N][N]){
    int cnt=0;
    for(int r=0;r<N;r++) for(int c=0;c<N;c++) if(board[r][c]==SHIP) cnt++;
    return cnt;
}

void flush_input(){
    int c; while((c=getchar())!='\n' && c!=EOF);
}

void coords_from_input(const char *s, int *r, int *c){
    // accept formats like A1 or a10
    if(!s || strlen(s)<2){ *r=*c=-1; return; }
    char col = toupper(s[0]);
    if(col<'A' || col>'A'+N-1){ *r=*c=-1; return; }
    int row = atoi(s+1);
    if(row<1 || row> N){ *r=*c=-1; return; }
    *r = col - 'A'; *c = row -1;
}

int cpu_choose_shot(Cell view[N][N], int *out_r, int *out_c){
    // simple random AI: choose random unseen cell
    int attempts=0;
    while(attempts<1000){
        int r = rand()%N; int c = rand()%N;
        if(view[r][c]!=HIT && view[r][c]!=MISS){ *out_r=r; *out_c=c; return 1; }
        attempts++;
    }
    // fallback deterministic search
    for(int r=0;r<N;r++) for(int c=0;c<N;c++) if(view[r][c]!=HIT && view[r][c]!=MISS){ *out_r=r; *out_c=c; return 1; }
    return 0;
}

int main(){
    srand((unsigned) time(NULL));
    Cell player[N][N], cpu[N][N], view_cpu[N][N], view_player[N][N];
    init_board(player); init_board(cpu); init_board(view_cpu); init_board(view_player);

    printf("=== BATALHA NAVAL (Player vs CPU) ===\n");
    printf("Tabuleiro %dx%d. Embarcações: %d\n", N, N, SHIP_COUNT);

    // Player placement
    printf("Deseja posicionar seus navios manualmente? (s/n): ");
    char ans[8]; if(!fgets(ans,sizeof(ans),stdin)) return 0;
    if(tolower(ans[0])=='s'){
        player_place_manual(player);
    } else {
        place_ships_random(player);
    }

    // CPU placement
    place_ships_random(cpu);

    // Main loop
    while(1){
        print_boards(player, view_cpu);
        printf("\nSua vez. Digite coordenada para atirar (ex: A5) ou 'sair' para encerrar: ");
        char line[64]; if(!fgets(line,sizeof(line),stdin)) break;
        // trim newline
        for(char *p=line; *p; ++p) if(*p=='\n') *p='\0';
        if(strcmp(line, "sair")==0) break;
        int r,c; coords_from_input(line, &r, &c);
        if(r<0){ printf("Entrada inválida. Use formato A1..J10.\n"); continue; }
        int res = attack(cpu, view_cpu, r, c);
        if(res==-1){ printf("Você já atirou nessa posição. Tente outra.\n"); continue; }
        if(res==1) {
            printf("Acertou!\n");
            if(ships_remaining(cpu)==0){
                printf("Parabéns! Você afundou todos os navios inimigos — você venceu!\n");
                print_boards(player, view_cpu);
                break;
            }
        } else printf("Água.\n");

        // CPU turn
        printf("Vez do CPU...\n");
        int cr, cc; if(!cpu_choose_shot(view_player, &cr, &cc)) { printf("CPU não tem onde atirar (empate?).\n"); break; }
        int rres = attack(player, view_player, cr, cc);
        if(rres==-1){ /* shouldn't happen */ }
        else if(rres==1){ printf("CPU acertou em %c%d!\n", 'A'+cr, cc+1); if(ships_remaining(player)==0){ printf("Você perdeu — todos seus navios foram afundados.\n"); print_boards(player, view_cpu); break; } }
        else printf("CPU errou em %c%d.\n", 'A'+cr, cc+1);
    }

    printf("Jogo encerrado. Obrigado por jogar!\n");
    return 0;
}
