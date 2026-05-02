#ifndef UNTITLED1_LOC_H
#define UNTITLED1_LOC_H


typedef enum e_orientation
{
    NORTH,
    EAST,
    SOUTH,
    WEST
} t_orientation;

typedef struct e_position
{
    int x;
    int y;
} t_position;


typedef struct s_localisation
{
    t_position      pos;
    t_orientation   ori;
} t_localisation;

t_localisation loc_init(int, int, t_orientation);
int isValidLocalisation(t_position, int, int);
t_position LEFT(t_position);
t_position RIGHT(t_position);
t_position UP(t_position);
t_position DOWN(t_position);

#endif 