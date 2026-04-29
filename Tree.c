//
// tree.c - Implémentation de l'arbre N-aire pour la sélection des mouvements
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* =========================================================
 * Création d'un noeud
 * ========================================================= */

t_node *createNode(int value, t_localisation loc, t_move mv, int move_index,
                   t_node *parent, int max_children)
{
    t_node *node = (t_node *)malloc(sizeof(t_node));
    if (!node) {
        fprintf(stderr, "Erreur : allocation noeud impossible\n");
        exit(1);
    }
    node->value       = value;
    node->loc         = loc;
    node->move        = mv;
    node->move_index  = move_index;
    node->parent      = parent;
    node->nb_children = 0;

    if (max_children > 0) {
        node->children = (t_node **)malloc(max_children * sizeof(t_node *));
        if (!node->children) {
            fprintf(stderr, "Erreur : allocation fils impossible\n");
            exit(1);
        }
        for (int i = 0; i < max_children; i++)
            node->children[i] = NULL;
    } else {
        node->children = NULL;
    }
    return node;
}

/* =========================================================
 * Construction récursive de l'arbre
 * ========================================================= */

void buildTree(t_node *node, t_move *available_moves, int *used,
               int nb_moves, int nb_choices,
               t_localisation current_loc, t_map map)
{
    // Cas de base : on a fait tous les choix -> c'est une feuille, rien à faire
    if (nb_choices == 0)
        return;

    // Cas d'élagage : si la valeur du noeud courant est >= 10000,
    // c'est une crevasse ou hors-carte -> on ne descend pas plus bas
    if (node->value >= 9999)
        return;

    // Cas d'arrêt anticipé : MARC est arrivé à la base !
    if (node->value == 0)
        return;

    int child_idx = 0;

    for (int i = 0; i < nb_moves; i++)
    {
        // On ne peut utiliser un mouvement qu'une seule fois par chemin
        if (used[i])
            continue;

        // Calculer la nouvelle localisation après ce mouvement
        t_localisation new_loc = move(current_loc, available_moves[i]);

        // Calculer la valeur de la case atteinte
        int new_value;
        if (!isValidLocalisation(new_loc.pos, map.x_max, map.y_max))
        {
            // Hors carte : coût maximal (MARC est perdu)
            new_value = 9999;
        }
        else
        {
            new_value = map.costs[new_loc.pos.y][new_loc.pos.x];
        }

        // Créer le fils
        int remaining = nb_moves - child_idx - 1; // mouvements restants pour les sous-fils
        // Le nombre max de fils au prochain niveau = nb_moves - (nb_moves - nb_choices + 1)
        int max_grandchildren = nb_moves - 1; // au pire, tous les autres mouvements
        t_node *child = createNode(new_value, new_loc, available_moves[i], i,
                                   node, max_grandchildren);
        node->children[child_idx] = child;
        node->nb_children++;
        child_idx++;

        // Marquer le mouvement comme utilisé dans ce chemin
        used[i] = 1;

        // Construire récursivement le sous-arbre
        buildTree(child, available_moves, used, nb_moves, nb_choices - 1, new_loc, map);

        // Démarquer pour le prochain frère
        used[i] = 0;
    }
}

/* =========================================================
 * Recherche de la feuille de valeur minimale
 * ========================================================= */

/**
 * @brief Fonction récursive interne : parcourt l'arbre et met à jour best_leaf
 */
static void findBestLeafRec(t_node *node, int current_depth, int target_depth,
                             t_node **best_leaf)
{
    if (!node) return;

    // On est à la profondeur cible : c'est une feuille
    if (current_depth == target_depth)
    {
        if (*best_leaf == NULL || node->value < (*best_leaf)->value)
        {
            *best_leaf = node;
        }
        return;
    }

    // Cas d'élagage : noeud de valeur >= 9999 -> pas de descendant valide
    if (node->value >= 9999)
        return;

    // Cas d'arrêt anticipé : valeur 0 = station de base atteinte !
    // On considère ce noeud comme la meilleure feuille possible
    if (node->value == 0)
    {
        if (*best_leaf == NULL || node->value < (*best_leaf)->value)
        {
            *best_leaf = node;
        }
        return;
    }

    // Parcourir tous les fils
    for (int i = 0; i < node->nb_children; i++)
    {
        findBestLeafRec(node->children[i], current_depth + 1, target_depth, best_leaf);
    }
}

t_node *findBestLeaf(t_node *root, int nb_choices)
{
    t_node *best = NULL;
    findBestLeafRec(root, 0, nb_choices, &best);
    return best;
}

/* =========================================================
 * Reconstruction du chemin racine -> feuille
 * ========================================================= */

void getPathFromLeaf(t_node *leaf, t_move *sequence, int nb_choices)
{
    // On remonte depuis la feuille jusqu'à la racine en stockant les mouvements
    // à l'envers, puis on inverse
    t_node *current = leaf;
    int depth = 0;

    // Remplir à l'envers
    while (current->parent != NULL)
    {
        sequence[nb_choices - 1 - depth] = current->move;
        depth++;
        current = current->parent;
    }
    // Note : si MARC atteint la base avant la fin (depth < nb_choices),
    // les premiers éléments du tableau ne sont pas définis, ce qui est ok
    // car on s'arrête dès que value == 0
}

/* =========================================================
 * Libération mémoire
 * ========================================================= */

void freeTree(t_node *node)
{
    if (!node) return;
    for (int i = 0; i < node->nb_children; i++)
    {
        freeTree(node->children[i]);
    }
    if (node->children) free(node->children);
    free(node);
}