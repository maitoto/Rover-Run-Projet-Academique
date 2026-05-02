
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "loc.h"
#include "queue.h"


t_position getBaseStationPosition(t_map);
void calculateCosts(t_map);
void removeFalseCrevasses(t_map);

t_position getBaseStationPosition(t_map map)
{
    t_position pos;
    int i = 0;
    int found = 0;
    while (i < map.y_max && !found)
    {
        int j = 0;
        while (j < map.x_max && !found)
        {
            if (map.soils[i][j] == BASE_STATION)
            {
                pos.x = j;
                pos.y = i;
                found = 1;
            }
            j++;
        }
        i++;
    }

    if (!found)
    {
        fprintf(stderr, "Error: base station not found in the map\n");
        exit(1);
    }
    return pos;
}

void removeFalseCrevasses(t_map map)
{
    int over=0;
    int imin, jmin;
    while (!over)
    {
        int min_cost = COST_UNDEF;
        imin = map.y_max;
        jmin = map.x_max;
        for (int i=0; i<map.y_max; i++)
        {
            for (int j=0; j<map.x_max; j++)
            {
                if (map.soils[i][j] != CREVASSE && map.costs[i][j] > 10000 && map.costs[i][j] < min_cost)
                {
                    min_cost = map.costs[i][j];
                    imin = i;
                    jmin = j;
                }
            }
        }
        if (imin < map.y_max && jmin < map.x_max)
        {
            t_position pos;
            pos.x = jmin;
            pos.y = imin;
            t_position lp, rp, up, dp;
            lp = LEFT(pos);
            rp = RIGHT(pos);
            up = UP(pos);
            dp = DOWN(pos);
            int min_neighbour = COST_UNDEF;
            if (isValidLocalisation(lp, map.x_max, map.y_max))
            {
                min_neighbour = (map.costs[lp.y][lp.x] < min_neighbour) ? map.costs[lp.y][lp.x] : min_neighbour;
            }
            if (isValidLocalisation(rp, map.x_max, map.y_max))
            {
                min_neighbour = (map.costs[rp.y][rp.x] < min_neighbour) ? map.costs[rp.y][rp.x] : min_neighbour;
            }
            if (isValidLocalisation(up, map.x_max, map.y_max))
            {
                min_neighbour = (map.costs[up.y][up.x] < min_neighbour) ? map.costs[up.y][up.x] : min_neighbour;
            }
            if (isValidLocalisation(dp, map.x_max, map.y_max))
            {
                min_neighbour = (map.costs[dp.y][dp.x] < min_neighbour) ? map.costs[dp.y][dp.x] : min_neighbour;
            }
            int self_cost = _soil_cost[map.soils[imin][jmin]];
            map.costs[imin][jmin] = (min_neighbour + self_cost < map.costs[imin][jmin]) ? min_neighbour + self_cost : map.costs[imin][jmin];
        }
        else
        {
            over = 1;
        }
    }
}

void calculateCosts(t_map map)
{
    t_position baseStation = getBaseStationPosition(map);
    t_queue queue = createQueue(map.x_max * map.y_max);
    enqueue(&queue, baseStation);
    while (queue.first != queue.last)
    {
        t_position pos = dequeue(&queue);
        int self_cost = _soil_cost[map.soils[pos.y][pos.x]];
        t_position lp, rp, up, dp;
        lp = LEFT(pos);
        rp = RIGHT(pos);
        up = UP(pos);
        dp = DOWN(pos);
        int min_cost = COST_UNDEF;
        if (isValidLocalisation(lp, map.x_max, map.y_max))
        {
            min_cost = (map.costs[lp.y][lp.x] < min_cost) ? map.costs[lp.y][lp.x] : min_cost;
        }
        if (isValidLocalisation(rp, map.x_max, map.y_max))
        {
            min_cost = (map.costs[rp.y][rp.x] < min_cost) ? map.costs[rp.y][rp.x] : min_cost;
        }
        if (isValidLocalisation(up, map.x_max, map.y_max))
        {
            min_cost = (map.costs[up.y][up.x] < min_cost) ? map.costs[up.y][up.x] : min_cost;
        }
        if (isValidLocalisation(dp, map.x_max, map.y_max))
        {
            min_cost = (map.costs[dp.y][dp.x] < min_cost) ? map.costs[dp.y][dp.x] : min_cost;
        }
        map.costs[pos.y][pos.x] = (map.soils[pos.y][pos.x] == BASE_STATION) ? 0 : min_cost + self_cost;
        if (isValidLocalisation(lp, map.x_max, map.y_max) && map.costs[lp.y][lp.x] == COST_UNDEF)
        {
            // mark as visited - change the cost to 65534
            map.costs[lp.y][lp.x] = COST_UNDEF-1;
            enqueue(&queue, lp);
        }
        if (isValidLocalisation(rp, map.x_max, map.y_max) && map.costs[rp.y][rp.x] == COST_UNDEF)
        {
            map.costs[rp.y][rp.x] = COST_UNDEF-1;
            enqueue(&queue, rp);
        }
        if (isValidLocalisation(up, map.x_max, map.y_max) && map.costs[up.y][up.x] == COST_UNDEF)
        {
            map.costs[up.y][up.x] = COST_UNDEF-1;
            enqueue(&queue, up);
        }
        if (isValidLocalisation(dp, map.x_max, map.y_max) && map.costs[dp.y][dp.x] == COST_UNDEF)
        {
            map.costs[dp.y][dp.x] = COST_UNDEF-1;
            enqueue(&queue, dp);
        }
    }


    return;
}

t_map createMapFromFile(char *filename)
{
    t_map map;
    int xdim, ydim;   
    char buffer[100];  

    FILE *file = fopen(filename,"rt");
    if (file == NULL)
    {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(1);
    }
    fscanf(file, "%d", &ydim);
    fscanf(file, "%d", &xdim);
    map.x_max = xdim;
    map.y_max = ydim;
    map.soils = (t_soil **)malloc(ydim * sizeof(t_soil *));
    for (int i = 0; i < ydim; i++)
    {
        map.soils[i] = (t_soil *)malloc(xdim * sizeof(t_soil));
    }
    map.costs = (int **)malloc(ydim * sizeof(int *));
    for (int i = 0; i < ydim; i++)
    {
        map.costs[i] = (int *)malloc(xdim * sizeof(int));
    }
    for (int i = 0; i < ydim; i++)
    {

        for (int j = 0; j < xdim; j++)
        {
            int value;
            fscanf(file, "%d", &value);
            map.soils[i][j] = value;
            map.costs[i][j] = (value == BASE_STATION) ? 0 : COST_UNDEF;
        }

    }
    fclose(file);
    calculateCosts(map);
    removeFalseCrevasses(map);
    return map;
}

t_map createTrainingMap()
{
    return createMapFromFile("..\\maps\\training.map");
}

void displayMap(t_map map)
{

    for (int i = 0; i < map.y_max; i++)
    {
        for (int rep = 0; rep < 3; rep++)
        {
            for (int j = 0; j < map.x_max; j++)
            {
                char c[4];
                switch (map.soils[i][j])
                {
                    case BASE_STATION:
                        if (rep==1)
                        {
                            strcpy(c, " B ");
                        }
                        else
                        {
                            strcpy(c, "   ");
                        }
                        break;
                    case PLAIN:
                        strcpy(c, "---");
                        break;
                    case ERG:
                        strcpy(c, "~~~");
                        break;
                    case REG:
                        strcpy(c, "^^^");
                        break;
                    case CREVASSE:
                        sprintf(c, "%c%c%c",219,219,219);
                        break;
                    default:
                        strcpy(c, "???");
                        break;
                }
                printf("%s", c);
            }
            printf("\n");
        }

    }
    return;
}