//
// phase.c - Tirage aléatoire des mouvements et gestion des phases
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "phase.h"

/* =========================================================
 * Tirage aléatoire des mouvements (Table 1)
 * ========================================================= */

void drawMoves(t_move *drawn_moves)
{
    // Copie des probabilités initiales (en "centièmes", i.e. 8 = 8%)
    // On travaille avec des entiers pour éviter les flottants
    int proba[NB_MOVE_TYPES];
    for (int i = 0; i < NB_MOVE_TYPES; i++)
        proba[i] = _initial_probabilities[i];

    for (int draw = 0; draw < NB_DRAWN_MOVES; draw++)
    {
        // Calculer la somme totale des probabilités actuelles
        int total = 0;
        for (int i = 0; i < NB_MOVE_TYPES; i++)
        {
            if (proba[i] > 0)
                total += proba[i];
        }

        // Tirer un nombre aléatoire entre 0 et total-1
        int r = rand() % total;

        // Trouver quel mouvement a été tiré
        int cumul = 0;
        int chosen = -1;
        for (int i = 0; i < NB_MOVE_TYPES; i++)
        {
            if (proba[i] <= 0) continue;
            cumul += proba[i];
            if (r < cumul)
            {
                chosen = i;
                break;
            }
        }

        // Enregistrer le mouvement tiré
        drawn_moves[draw] = (t_move)chosen;

        // Diminuer la probabilité du mouvement tiré de 1
        if (proba[chosen] > 0)
            proba[chosen]--;
    }
}

/* =========================================================
 * Affichage des mouvements tirés
 * ========================================================= */

void displayDrawnMoves(t_move *drawn_moves, int nb_moves)
{
    printf("Mouvements disponibles pour cette phase :\n");
    for (int i = 0; i < nb_moves; i++)
    {
        printf("  [%d] %s\n", i, getMoveAsString(drawn_moves[i]));
    }
}

/* =========================================================
 * Exécution d'une phase
 * ========================================================= */

int runPhase(t_localisation *loc, t_map map, int nb_choices)
{
    printf("\n=== Nouvelle phase (MARC en [%d,%d] orient:%d, coût:%d) ===\n",
           loc->pos.x, loc->pos.y, loc->ori,
           map.costs[loc->pos.y][loc->pos.x]);

    // 1. Tirer les 9 mouvements aléatoirement
    t_move drawn_moves[NB_DRAWN_MOVES];
    drawMoves(drawn_moves);
    displayDrawnMoves(drawn_moves, NB_DRAWN_MOVES);

    // 2. Créer la racine de l'arbre avec la valeur de la case courante
    int start_value = map.costs[loc->pos.y][loc->pos.x];
    t_node *root = createNode(start_value, *loc, (t_move)0, -1, NULL, NB_DRAWN_MOVES);

    // 3. Tableau "used" : quel mouvement a déjà été utilisé dans le chemin courant
    int used[NB_DRAWN_MOVES];
    memset(used, 0, sizeof(used));

    // 4. Construire l'arbre récursivement
    buildTree(root, drawn_moves, used, NB_DRAWN_MOVES, nb_choices, *loc, map);

    // 5. Trouver la feuille de valeur minimale
    t_node *best_leaf = findBestLeaf(root, nb_choices);

    if (best_leaf == NULL)
    {
        printf("Aucune feuille trouvée ! MARC est coincé.\n");
        freeTree(root);
        return -1;
    }

    printf("Meilleure valeur de case atteinte : %d\n", best_leaf->value);

    // 6. Récupérer la séquence de mouvements
    // Calculer la profondeur réelle de la feuille trouvée
    int depth = 0;
    t_node *tmp = best_leaf;
    while (tmp->parent != NULL) { depth++; tmp = tmp->parent; }

    t_move sequence[NB_CHOICES_NORMAL];
    getPathFromLeaf(best_leaf, sequence, depth);

    printf("Séquence de mouvements choisie :\n");
    for (int i = 0; i < depth; i++)
        printf("  %d. %s\n", i+1, getMoveAsString(sequence[i]));

    // 7. Appliquer la séquence à MARC
    int reached_base = 0;
    int lost = 0;
    for (int i = 0; i < depth; i++)
    {
        updateLocalisation(loc, sequence[i]);
        printf("  -> Après %s : MARC en [%d,%d]\n",
               getMoveAsString(sequence[i]), loc->pos.x, loc->pos.y);

        // Vérifier si MARC est sorti de la carte
        if (!isValidLocalisation(loc->pos, map.x_max, map.y_max))
        {
            printf("  !! MARC est sorti de la carte !! Mission échouée.\n");
            lost = 1;
            break;
        }

        int cell_cost = map.costs[loc->pos.y][loc->pos.x];
        int cell_soil = map.soils[loc->pos.y][loc->pos.x];

        // Vérifier crevasse
        if (cell_soil == CREVASSE)
        {
            printf("  !! MARC est tombé dans une crevasse !! Mission échouée.\n");
            lost = 1;
            break;
        }

        // Vérifier station de base
        if (cell_cost == 0)
        {
            printf("  *** MARC a atteint la station de base ! Mission accomplie ! ***\n");
            reached_base = 1;
            break;
        }
    }

    freeTree(root);

    if (lost) return -1;
    if (reached_base) return 1;

    // Retourner le nombre de choices pour la prochaine phase
    // (4 si MARC est sur un Reg, 5 sinon)
    return 0;
}

/* =========================================================
 * Boucle principale de guidage
 * ========================================================= */

void runMission(t_localisation *loc, t_map map, int max_phases)
{
    // Initialiser le générateur aléatoire
    srand((unsigned int)time(NULL));

    int nb_choices = NB_CHOICES_NORMAL;
    int phase = 0;
    int result = 0;

    printf("\n*** DEBUT DE LA MISSION ***\n");
    printf("Position initiale : [%d,%d], orientation : %d\n",
           loc->pos.x, loc->pos.y, loc->ori);
    printf("Coût de la case initiale : %d\n",
           map.costs[loc->pos.y][loc->pos.x]);

    while (phase < max_phases)
    {
        phase++;
        printf("\n--- Phase %d ---\n", phase);

        result = runPhase(loc, map, nb_choices);

        if (result == 1)
        {
            printf("\n*** Mission accomplie en %d phase(s) ! ***\n", phase);
            return;
        }
        if (result == -1)
        {
            printf("\n*** Mission échouée après %d phase(s). ***\n", phase);
            return;
        }

        // Vérifier si la prochaine phase a 4 ou 5 mouvements (selon le terrain actuel)
        if (isValidLocalisation(loc->pos, map.x_max, map.y_max))
        {
            if (map.soils[loc->pos.y][loc->pos.x] == REG)
            {
                printf("(MARC est sur un Reg : phase suivante limitée à 4 mouvements)\n");
                nb_choices = NB_CHOICES_REG;
            }
            else
            {
                nb_choices = NB_CHOICES_NORMAL;
            }
        }
    }

    printf("\nNombre maximum de phases atteint (%d). Mission interrompue.\n", max_phases);
}