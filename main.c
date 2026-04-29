#include <stdio.h>
#include <time.h>
#include "map.h"
#include "moves.h"
#include "loc.h"
#include "phase.h"

int main()
{
    t_map map;

    // Charger la carte selon l'OS
#if defined(_WIN32) || defined(_WIN64)
    map = createMapFromFile(".\\maps\\example1.map");
#else
    map = createMapFromFile("./maps/example1.map");
#endif

    printf("Carte chargée : %d x %d\n", map.x_max, map.y_max);
    displayMap(map);

    printf("\nCoûts calculés :\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++)
            printf("%-6d", map.costs[i][j]);
        printf("\n");
    }

    // --- Position initiale de MARC ---
    // Sur la carte example1.map, la base est en [2,1] (col 2, ligne 1)
    // On place MARC en bas à droite, case [5,6], face au NORD
    t_localisation marc = loc_init(5, 6, NORTH);

    // Lancer la mission (max 50 phases pour éviter une boucle infinie)
    runMission(&marc, map, 50);

    return 0;
}