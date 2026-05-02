#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "phase.h"

void drawMoves(t_move *drawn_moves)
{
    int proba[NB_MOVE_TYPES];
    for (int i = 0; i < NB_MOVE_TYPES; i++)
        proba[i] = _initial_probabilities[i];

    for (int draw = 0; draw < NB_DRAWN_MOVES; draw++)
    {
        int total = 0;
        for (int i = 0; i < NB_MOVE_TYPES; i++)
            if (proba[i] > 0) total += proba[i];

        int r = rand() % total;
        int cumul = 0;
        int chosen = -1;
        for (int i = 0; i < NB_MOVE_TYPES; i++)
        {
            if (proba[i] <= 0) continue;
            cumul += proba[i];
            if (r < cumul) { chosen = i; break; }
        }
        drawn_moves[draw] = (t_move)chosen;
        if (proba[chosen] > 0) proba[chosen]--;
    }
}

void displayDrawnMoves(t_move *drawn_moves, int nb_moves)
{
    printf("Mouvements disponibles :\n");
    for (int i = 0; i < nb_moves; i++)
        printf("  [%d] %s\n", i, getMoveAsString(drawn_moves[i]));
}

void printSummary(t_history *history, int success)
{
    printf("\n========================================\n");
    printf("         RAPPORT DE MISSION\n");
    printf("========================================\n");
    printf("Depart  : [%d,%d]\n",
           history->positions[0].x,
           history->positions[0].y);
    printf("Arrivee : [%d,%d]\n",
           history->positions[history->nb_steps-1].x,
           history->positions[history->nb_steps-1].y);
    printf("Cases visitees : %d\n\n", history->nb_steps);

    printf("Trajet complet :\n");
    for (int i = 0; i < history->nb_steps; i++)
    {
        printf("  Etape %2d : [%d,%d] cout=%d",
               i+1,
               history->positions[i].x,
               history->positions[i].y,
               history->costs[i]);
        if (i == 0)                    printf(" <- DEPART");
        if (history->costs[i] == 0)    printf(" <- BASE !");
        printf("\n");
    }

    printf("\nResultat : %s\n",
           success ? "MISSION ACCOMPLIE !" : "Mission echouee.");
    printf("========================================\n");
}

int runPhase(t_localisation *loc, t_map map, int nb_choices, t_history *history)
{
    printf("\n=== Phase (MARC en [%d,%d] cout:%d) ===\n",
           loc->pos.x, loc->pos.y,
           map.costs[loc->pos.y][loc->pos.x]);

    t_move drawn_moves[NB_DRAWN_MOVES];
    drawMoves(drawn_moves);
    displayDrawnMoves(drawn_moves, NB_DRAWN_MOVES);

    int start_value = map.costs[loc->pos.y][loc->pos.x];
    t_node *root = createNode(start_value, *loc, (t_move)0, -1, NULL, NB_DRAWN_MOVES);

    int used[NB_DRAWN_MOVES];
    memset(used, 0, sizeof(used));
    buildTree(root, drawn_moves, used, NB_DRAWN_MOVES, nb_choices, *loc, map);

    t_node *best_leaf = findBestLeaf(root, nb_choices);
    if (best_leaf == NULL)
    {
        printf("Aucune feuille trouvee ! MARC est coince.\n");
        freeTree(root);
        return -1;
    }

    printf("Meilleure valeur atteinte : %d\n", best_leaf->value);

    int depth = 0;
    t_node *tmp = best_leaf;
    while (tmp->parent != NULL) { depth++; tmp = tmp->parent; }

    t_move sequence[NB_CHOICES_NORMAL];
    getPathFromLeaf(best_leaf, sequence, depth);

    printf("Sequence choisie :\n");
    for (int i = 0; i < depth; i++)
        printf("  %d. %s\n", i+1, getMoveAsString(sequence[i]));

    int reached_base = 0;
    int lost = 0;

    for (int i = 0; i < depth; i++)
    {
        updateLocalisation(loc, sequence[i]);
        printf("  -> Apres %s : MARC en [%d,%d]\n",
               getMoveAsString(sequence[i]), loc->pos.x, loc->pos.y);

        if (!isValidLocalisation(loc->pos, map.x_max, map.y_max))
        {
            printf("  !! MARC est sorti de la carte !!\n");
            lost = 1;
            break;
        }

        if (history->nb_steps < MAX_STEPS)
        {
            history->positions[history->nb_steps] = loc->pos;
            history->costs[history->nb_steps]     = map.costs[loc->pos.y][loc->pos.x];
            history->nb_steps++;
        }

        if (map.soils[loc->pos.y][loc->pos.x] == CREVASSE)
        {
            printf("  !! MARC est tombe dans une crevasse !!\n");
            lost = 1;
            break;
        }

        if (map.costs[loc->pos.y][loc->pos.x] == 0)
        {
            printf("  *** MARC a atteint la base ! ***\n");
            reached_base = 1;
            break;
        }
    }

    freeTree(root);
    if (lost)         return -1;
    if (reached_base) return 1;
    return 0;
}

void runMission(t_localisation *loc, t_map map, int max_phases)
{
    srand((unsigned int)time(NULL));

    t_history history;
    history.nb_steps     = 1;
    history.positions[0] = loc->pos;
    history.costs[0]     = map.costs[loc->pos.y][loc->pos.x];

    int nb_choices = NB_CHOICES_NORMAL;
    int phase      = 0;
    int result     = 0;

    printf("\n*** DEBUT DE LA MISSION ***\n");
    printf("Depart : [%d,%d], orientation : %d\n",
           loc->pos.x, loc->pos.y, loc->ori);

    while (phase < max_phases)
    {
        phase++;
        printf("\n--- Phase %d ---\n", phase);

        result = runPhase(loc, map, nb_choices, &history);

        if (result == 1)
        {
            printf("\n*** Mission accomplie en %d phase(s) ! ***\n", phase);
            printSummary(&history, 1);
            return;
        }
        if (result == -1)
        {
            printf("\n*** Mission echouee apres %d phase(s). ***\n", phase);
            printSummary(&history, 0);
            return;
        }

        if (isValidLocalisation(loc->pos, map.x_max, map.y_max))
        {
            if (map.soils[loc->pos.y][loc->pos.x] == REG)
            {
                printf("(MARC sur Reg : 4 mouvements phase suivante)\n");
                nb_choices = NB_CHOICES_REG;
            }
            else
            {
                nb_choices = NB_CHOICES_NORMAL;
            }
        }
    }

    printf("\nNombre max de phases atteint.\n");
    printSummary(&history, 0);
}