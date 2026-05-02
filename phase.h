#ifndef ROVERRUN_PHASE_H
#define ROVERRUN_PHASE_H

#include "moves.h"
#include "map.h"
#include "tree.h"

#define NB_MOVE_TYPES    7
#define NB_DRAWN_MOVES   9
#define NB_CHOICES_NORMAL 5
#define NB_CHOICES_REG   4
#define MAX_STEPS        200

static const int _initial_probabilities[NB_MOVE_TYPES] = {8, 4, 3, 2, 5, 5, 3};

typedef struct s_history {
    t_position positions[MAX_STEPS];
    int        costs[MAX_STEPS];
    int        nb_steps;
} t_history;

void drawMoves(t_move *drawn_moves);
void displayDrawnMoves(t_move *drawn_moves, int nb_moves);
int  runPhase(t_localisation *loc, t_map map, int nb_choices, t_history *history);
void runMission(t_localisation *loc, t_map map, int max_phases);
void printSummary(t_history *history, int success);

#endif