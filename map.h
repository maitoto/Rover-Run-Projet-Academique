
#ifndef UNTITLED1_MAP_H
#define UNTITLED1_MAP_H

#define COST_UNDEF 65535

typedef enum e_soil
{
    BASE_STATION,
    PLAIN,
    ERG,
    REG,
    CREVASSE
} t_soil;


static const int _soil_cost[5] = {0, 1, 2, 4, 10000};


typedef struct s_map
{
    t_soil  **soils;
    int     **costs;
    int     x_max;
    int     y_max;
} t_map;


t_map createMapFromFile(char *);
 t_map createTrainingMap();
void displayMap(t_map);

#endif 