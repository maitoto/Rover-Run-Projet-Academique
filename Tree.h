//
// tree.h - Arbre N-aire pour la sélection des mouvements de MARC
//

#ifndef ROVERRUN_TREE_H
#define ROVERRUN_TREE_H

#include "moves.h"
#include "map.h"

/**
 * @brief Noeud de l'arbre N-aire
 *
 * L'arbre représente toutes les séquences possibles de mouvements pour une phase.
 * - La racine (level=0) contient la valeur de la case de départ
 * - Chaque niveau correspond au choix d'un mouvement
 * - Chaque noeud a jusqu'à NB_MOVES fils (un par mouvement disponible)
 * - Les feuilles (level=NB_CHOICES) contiennent la valeur de la case atteinte
 */
typedef struct s_node {
    int             value;          // coût de la case atteinte après ce mouvement
    t_localisation  loc;            // localisation de MARC après ce mouvement
    t_move          move;           // mouvement qui a amené ici (non défini pour la racine)
    int             move_index;     // index du mouvement dans le tableau des mouvements dispo
    struct s_node   **children;     // tableau de fils (taille = nb mouvements restants)
    int             nb_children;    // nombre de fils effectivement créés
    struct s_node   *parent;        // pointeur vers le père (pour remonter le chemin)
} t_node;

/**
 * @brief Arbre N-aire complet pour une phase
 */
typedef struct s_tree {
    t_node  *root;          // racine de l'arbre
    int     nb_choices;     // nombre de mouvements à choisir (5 en général, 4 sur Reg)
    int     nb_moves;       // nombre de mouvements disponibles (9 tirés au hasard)
} t_tree;

/**
 * @brief Créer un noeud de l'arbre
 * @param value : coût de la case
 * @param loc : localisation associée
 * @param move : mouvement utilisé pour arriver ici
 * @param move_index : index du mouvement dans le tableau
 * @param parent : noeud parent
 * @param max_children : nombre max de fils possibles
 * @return le noeud créé (alloué dynamiquement)
 */
t_node *createNode(int value, t_localisation loc, t_move mv, int move_index,
                   t_node *parent, int max_children);

/**
 * @brief Construire récursivement l'arbre des séquences de mouvements
 * @param node : noeud courant
 * @param available_moves : tableau des mouvements disponibles pour cette phase
 * @param used : tableau booléen (1 = mouvement déjà utilisé dans ce chemin)
 * @param nb_moves : nombre total de mouvements disponibles
 * @param nb_choices : nombre de mouvements encore à choisir
 * @param current_loc : localisation courante de MARC
 * @param map : la carte Mars
 */
void buildTree(t_node *node, t_move *available_moves, int *used,
               int nb_moves, int nb_choices,
               t_localisation current_loc, t_map map);

/**
 * @brief Trouver la feuille de valeur minimale dans l'arbre
 * @param root : racine de l'arbre
 * @param nb_choices : profondeur des feuilles
 * @return pointeur vers la feuille de valeur minimale
 */
t_node *findBestLeaf(t_node *root, int nb_choices);

/**
 * @brief Récupérer la séquence de mouvements depuis la racine jusqu'à une feuille
 * @param leaf : la feuille cible
 * @param sequence : tableau à remplir (doit avoir la taille nb_choices)
 * @param nb_choices : nombre de mouvements dans la séquence
 */
void getPathFromLeaf(t_node *leaf, t_move *sequence, int nb_choices);

/**
 * @brief Libérer récursivement la mémoire de l'arbre
 * @param node : noeud à libérer (et tous ses descendants)
 */
void freeTree(t_node *node);

#endif //ROVERRUN_TREE_H