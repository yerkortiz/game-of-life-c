/* 
    Author: Yerko Ortiz 
    Year: 2020
    Conway's game of life implemented using an array of bitfields.
    For future work would be interesting increase the maximum size of the grid
    and upgrade the visualization of the automaton, but by the moment
    this works perfect.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//typedef unsigned long long int uint64_t;

#define XORBIT(bit_field, index) { bit_field ^= (1llu << index); }
#define CURRENTSTATE(bit_field, index, state) { state = (bit_field & (1llu << index)); }
#define AND  &&
#define OR ||
#define NOT !

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
void showAutomaton(uint64_t *automaton, int n, int m)
{
    printf("\033[H\033[E\033[E\033[E");
    int i, j, state;
    for(i = 0; i < n; ++i) {
        for(j = 0; j < m; ++j) {
            if(NOT j) printf("     ");
            CURRENTSTATE(automaton[i], j, state);
            //printf(state ? "\033[07m  \033[m" : "  ");
            printf(state ? "\033[07m ": " ")
        } 
        printf("\033[E");
    }
    printf("\033[E");
    fflush(stdout);
}
void rndStart(uint64_t *automaton, int n, int m) 
{
    srand(rdtsc());
    int i; 
    uint64_t range = (1 << (m + 1));
    for(i = 0; i < n; ++i) {
        uint64_t rnd = rand() % range;
        automaton[i] |= rnd;
    }
}
void rndFixedStart(uint64_t *automaton, int n, int m, int f)
{
    srand(rdtsc());
    while(f > 0) {
        int i = rand() % n, j = rand() % m;
        XORBIT(automaton[i], j);
        --f;
    }
}
int mooreNeighborhood(uint64_t *automaton, int x, int y, int n, int m)
{
    int neighbors, i, j, state, endI, endJ, startI, startJ;
    neighbors = 0;
    startI = (x - 1) < 0 ? 0 : x - 1;
    startJ = (y - 1) < 0 ? 0 : y - 1;
    endI = (x + 1) >= n ? n : x + 2;
    endJ = (y + 1) >= m ? m : y + 2;
    for(i = startI; i < endI; ++i) {
        for(j = startJ; j < endJ; ++j) {
            if(i == x AND j == y) continue;
            CURRENTSTATE(automaton[i], j, state);  
            if(state) ++neighbors;     
        }
    }
    return neighbors;
}
void rule(uint64_t *automaton, int n, int m) 
{
    int i, j, neighbors, state;
    for(i = 0; i < n; ++i)
        for(j = 0; j < m; ++j){
            neighbors = mooreNeighborhood(automaton, i, j, n, m);
            CURRENTSTATE(automaton[i], j, state);
            /* come back to death */
            if(state AND (neighbors < 2 OR neighbors > 3)) XORBIT(automaton[i], j);
            /* come back to life */
            if(NOT state AND (neighbors == 2 OR neighbors == 3)) XORBIT(automaton[i], j);
        }
}
void gameOfLife(uint64_t *automaton, int n, int m, int f)
{
    if (f) rndFixedStart(automaton, n, m, f);
    else rndFixedStart(automaton, n, m, n);
	while (1) {
		showAutomaton(automaton, n, m);
		rule(automaton, n, m);
		usleep(2000000);
	}
}
int main(int argv, char **argc)
{
    /* se codifica el automata como un arreglo de bitfields */
    uint64_t automaton[64] = {0};
    gameOfLife(automaton, 10, 10, 0);
    return 0;
}