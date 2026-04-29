//
// phase.h - Tirage aléatoire des mouvements et gestion des phases
//

#ifndef ROVERRUN_PHASE_H
#define ROVERRUN_PHASE_H

#include "moves.h"
#include "map.h"
#include "tree.h"

/**
 * @brief Nombre total de types de mouvements disponibles
 */
#define NB_MOVE_TYPES 7

/**
 * @brief Nombre de mouvements tirés par phase
 */
#define NB_DRAWN_MOVES 9

/**
 * @brief Nombre de mouvements choisis par phase (normal)
 */
#define NB_CHOICES_NORMAL 5

/**
 * @brief Nombre de mouvements choisis par phase (si MARC finit sur un Reg)
 */
#define NB_CHOICES_REG 4

/**
 * @brief Probabilités initiales de tirage pour chaque mouvement (Table 1)
 * Ordre : F_10, F_20, F_30, B_10, T_LEFT, T_RIGHT, U_TURN
 */
static const int _initial_probabilities[NB_MOVE_TYPES] = {8, 4, 3, 2, 5, 5, 3};

/**
 * @brief Tirer aléatoirement NB_DRAWN_MOVES mouvements parmi les 7 types
 * selon les probabilités de la Table 1.
 * Chaque fois qu'un mouvement est tiré, sa probabilité diminue de 1%.
 * Les probabilités sont réinitialisées au début de chaque phase.
 *
 * @param drawn_moves : tableau de sortie, doit être de taille NB_DRAWN_MOVES
 */
void drawMoves(t_move *drawn_moves);

/**
 * @brief Afficher les mouvements tirés pour une phase
 * @param drawn_moves : tableau des mouvements tirés
 * @param nb_moves : nombre de mouvements dans le tableau
 */
void displayDrawnMoves(t_move *drawn_moves, int nb_moves);

/**
 * @brief Exécuter une phase complète :
 * 1. Tirer les 9 mouvements
 * 2. Construire l'arbre des séquences
 * 3. Trouver la meilleure feuille
 * 4. Appliquer la séquence de mouvements à MARC
 *
 * @param loc : pointeur vers la localisation courante de MARC (modifiée en place)
 * @param map : la carte Mars
 * @param nb_choices : nombre de mouvements à choisir (5 ou 4)
 * @return 1 si MARC a atteint la base, 0 sinon, -1 si MARC est perdu (hors carte ou crevasse)
 */
int runPhase(t_localisation *loc, t_map map, int nb_choices);

/**
 * @brief Boucle principale de guidage de MARC vers la station de base
 * @param loc : localisation initiale de MARC
 * @param map : la carte Mars
 * @param max_phases : nombre maximum de phases (sécurité anti-boucle infinie)
 */
void runMission(t_localisation *loc, t_map map, int max_phases);

#endif //ROVERRUN_PHASE_H