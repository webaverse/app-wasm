#include "march.h"
#include "subparcel.h"
#include "collide.h"
#include "vector.h"
#include "biomes.h"
#include <cstdlib>
#include <array>
#include <map>
#include <iostream>

int edgeTable[256]={
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };
int triTable[256][16] =
{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};
int cubeVerts[8][3] = {
  {0,0,0},
  {1,0,0},
  {1,1,0},
  {0,1,0},
  {0,0,1},
  {1,0,1},
  {1,1,1},
  {0,1,1}
};
int edgeIndex[12][2] = {
  {0,1},
  {1,2},
  {2,3},
  {3,0},
  {4,5},
  {5,6},
  {6,7},
  {7,4},
  {0,4},
  {1,5},
  {2,6},
  {3,7}
};

inline void _floodFill(int x, int y, int z, int startFace, std::function<float(int, int, int)> getPotential, int minX, int minY, int minZ, int maxX, int maxY, int maxZ, unsigned char *peeks, unsigned char *seenPeeks, int dimsP1[3]) {
  const int peekIndex = x +
    z * dimsP1[0] +
    y * dimsP1[0] * dimsP1[1];
  if (!seenPeeks[peekIndex]) {
    seenPeeks[peekIndex] = 1;

    std::vector<int> queue(dimsP1[0] * dimsP1[1] * dimsP1[2] * 3);
    unsigned int queueEnd = 0;
    queue[queueEnd * 3 + 0] = x;
    queue[queueEnd * 3 + 1] = y;
    queue[queueEnd * 3 + 2] = z;
    queueEnd++;

    int seenFaces[6+1];
    memset(seenFaces, 0, sizeof(seenFaces));
    seenFaces[startFace] = 1;

    for (unsigned int queueStart = 0; queueStart < queueEnd; queueStart++) {
      const int x = queue[queueStart * 3 + 0];
      const int y = queue[queueStart * 3 + 1];
      const int z = queue[queueStart * 3 + 2];

      if (getPotential(x, y, z) <= 0) { // if empty space
        if (z == minZ && startFace != (int)PEEK_FACES::BACK) {
          seenFaces[(int)PEEK_FACES::BACK] = 1;
          // peeks[PEEK_FACE_INDICES[startFace << 4 | (int)PEEK_FACES::BACK]] = 1;
        }
        if (z == maxZ-1 && startFace != (int)PEEK_FACES::FRONT) {
          seenFaces[(int)PEEK_FACES::FRONT] = 1;
          // peeks[PEEK_FACE_INDICES[startFace << 4 | (int)PEEK_FACES::FRONT]] = 1;
        }
        if (x == minX && startFace != (int)PEEK_FACES::LEFT) {
          seenFaces[(int)PEEK_FACES::LEFT] = 1;
          // peeks[PEEK_FACE_INDICES[startFace << 4 | (int)PEEK_FACES::LEFT]] = 1;
        }
        if (x == maxX-1 && startFace != (int)PEEK_FACES::RIGHT) {
          seenFaces[(int)PEEK_FACES::RIGHT] = 1;
          // peeks[PEEK_FACE_INDICES[startFace << 4 | (int)PEEK_FACES::RIGHT]] = 1;
        }
        if (y == maxY-1 && startFace != (int)PEEK_FACES::TOP) {
          seenFaces[(int)PEEK_FACES::TOP] = 1;
          // peeks[PEEK_FACE_INDICES[startFace << 4 | (int)PEEK_FACES::TOP]] = 1;
        }
        if (y == minY && startFace != (int)PEEK_FACES::BOTTOM) {
          seenFaces[(int)PEEK_FACES::BOTTOM] = 1;
          // peeks[PEEK_FACE_INDICES[startFace << 4 | (int)PEEK_FACES::BOTTOM]] = 1;
        }

        for (int dx = -1; dx <= 1; dx++) {
          const int ax = x + dx;
          if (ax >= minX && ax < maxX) {
            for (int dz = -1; dz <= 1; dz++) {
              const int az = z + dz;
              if (az >= minZ && az < maxZ) {
                for (int dy = -1; dy <= 1; dy++) {
                  const int ay = y + dy;
                  if (ay >= minY && ay < maxY) {
                    const int peekIndex = ax +
                      az * dimsP1[0] +
                      ay * dimsP1[0] * dimsP1[1];
                    if (!seenPeeks[peekIndex]) {
                      queue[queueEnd * 3 + 0] = ax;
                      queue[queueEnd * 3 + 1] = ay;
                      queue[queueEnd * 3 + 2] = az;
                      queueEnd++;
                      seenPeeks[peekIndex] = 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    for (int i = 1; i <= 6; i++) {
      if (seenFaces[i]) {
        for (int j = 1; j <= 6; j++) {
          if (i != j && seenFaces[j]) {
            peeks[PEEK_FACE_INDICES[i << 4 | j]] = 1;
          }
        }
      }
    }
  }
}

inline int getPotentialIndex(int x, int y, int z, int dimsP3[3]) {
  return (x + 1) +
    (z + 1) * dimsP3[0] +
    (y + 1) * dimsP3[0] * dimsP3[1];
}
inline char maxChar(char a, char b) {
  return a >= b ? a : b;
}

inline void setLights(const std::array<float, 3> &v, std::function<unsigned char(int)> getField, unsigned char *lights, unsigned int lightIndex, int dims[3]) {
  int x = (int)std::floor(v[0]);
  int y = (int)std::floor(v[1]);
  int z = (int)std::floor(v[2]);
  int index = x +
    (z * dims[0]) +
    (y * dims[0] * dims[1]);
  lights[lightIndex] = getField(index);
}
inline void setUvs(const std::tuple<float, float> &color, float *uvs, unsigned int uvIndex) {
  uvs[uvIndex] = std::get<0>(color);
  uvs[uvIndex+1] = std::get<1>(color);
}
inline unsigned char getAo(int x, int y, int z, std::function<float(int, int, int)> getPotential) {
  unsigned char numOpens = 0;
  for(int dy = -1; dy <= 1; dy++) {
    int ay = y + dy;
    for(int dz = -1; dz <= 1; dz++) {
      int az = z + dz;
      for(int dx = -1; dx <= 1; dx++) {
        int ax = x + dx;
        float potential = getPotential(ax, ay, az);
        if (potential < 0) {
          numOpens++;
        }
      }
    }
  }
  return numOpens;
}

template<bool transparent>
inline void marchingCubesRaw(float meshId, int dimsP1[3], std::function<float(int, int, int)> getPotential, std::function<unsigned char(int)> getBiome, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float yLimit, float *positions, float *normals, float *uvs, /*float *barycentrics,*/ unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int &positionIndex, unsigned int &normalIndex, unsigned int &uvIndex, /*unsigned int &barycentricIndex,*/ unsigned int &aoIndex, unsigned int &idIndex, unsigned int &skyLightsIndex, unsigned int &torchLightsIndex, unsigned char *peeks) {
  int n = 0;
  float grid[8] = {0};
  std::array<std::array<float, 3>, 12> edges;
  int x[3] = {0};

  //March over the volume
  for(x[2]=0; x[2]<dimsP1[2]-1; ++x[2], n+=dimsP1[0])
  for(x[1]=0; x[1]<dimsP1[1]-1; ++x[1], ++n)
  for(x[0]=0; x[0]<dimsP1[0]-1; ++x[0], ++n) {
    //For each cell, compute cube mask
    int cube_index = 0;
    for(int i=0; i<8; ++i) {
      int *v = cubeVerts[i];
      float s = getPotential(x[0]+v[0], x[1]+v[1], x[2]+v[2]);
      grid[i] = s;
      cube_index |= (s > 0) ? 1 << i : 0;
    }
    //Compute vertices
    int edge_mask = edgeTable[cube_index];
    if(edge_mask == 0) {
      continue;
    }
    for(int i=0; i<12; ++i) {
      if((edge_mask & (1<<i)) == 0) {
        continue;
      }
      int *e = edgeIndex[i];
      int *p0 = cubeVerts[e[0]];
      int *p1 = cubeVerts[e[1]];
      float a = grid[e[0]];
      float b = grid[e[1]];
      float d = a - b;
      float t = a / d;
      std::array<float, 3> &v = edges[i];
      for(int j=0; j<3; ++j) {
        v[j] = (x[j] + p0[j]) + t * (p1[j] - p0[j]);
      }
    }
    //Add faces
    int *f = triTable[cube_index];
    for(int i=0;f[i]!=-1;i+=3) {
      std::array<float, 3> &a = edges[f[i]];
      std::array<float, 3> &b = edges[f[i+2]];
      std::array<float, 3> &c = edges[f[i+1]];

      if (!transparent) {
        setLights(a, [&](int index) -> unsigned char { return (unsigned char)maxChar(heightfield[index], 0); }, skyLights, skyLightsIndex++, dimsP1);
        setLights(a, [&](int index) -> unsigned char { return lightfield[index]; }, torchLights, torchLightsIndex++, dimsP1);

        setLights(b, [&](int index) -> unsigned char { return (unsigned char)maxChar(heightfield[index], 0); }, skyLights, skyLightsIndex++, dimsP1);
        setLights(b, [&](int index) -> unsigned char { return lightfield[index]; }, torchLights, torchLightsIndex++, dimsP1);

        setLights(c, [&](int index) -> unsigned char { return (unsigned char)maxChar(heightfield[index], 0); }, skyLights, skyLightsIndex++, dimsP1);
        setLights(c, [&](int index) -> unsigned char { return lightfield[index]; }, torchLights, torchLightsIndex++, dimsP1);
      } else {
        constexpr unsigned char skyLight = 8;
        constexpr unsigned char torchLight = 0.0f;

        skyLights[skyLightsIndex++] = skyLight;
        torchLights[torchLightsIndex++] = torchLight;

        skyLights[skyLightsIndex++] = skyLight;
        torchLights[torchLightsIndex++] = torchLight;

        skyLights[skyLightsIndex++] = skyLight;
        torchLights[torchLightsIndex++] = torchLight;
      }
      {
        Tri tri(
          Vec(a[0], a[1], a[2]),
          Vec(b[0], b[1], b[2]),
          Vec(c[0], c[1], c[2])
        );
        Vec center = tri.midpoint();
        // Vec normal = tri.normal();
        // Vec point = center;// - normal;
        int x = (int)center.x;
        int y = (int)center.y;
        int z = (int)center.z;
        // Vec center(std::min({a[0], b[0], c[0]}), std::min({a[1], b[1], c[1]}), std::min({a[2], b[2], c[2]}));
        int biomeIndex = x +
          (z * dimsP1[0]);
        int biome = (int)getBiome(biomeIndex);
        const std::tuple<float, float> &color = groundColors[biome];

        setUvs(color, uvs, uvIndex);
        // setUvs(a, biomes, groundNormals, uvs, uvIndex, dimsP1);
        uvIndex += 2;
        setUvs(color, uvs, uvIndex);
        // setUvs(b, biomes, groundNormals, uvs, uvIndex, dimsP1);
        uvIndex += 2;
        setUvs(color, uvs, uvIndex);
        // setUvs(c, biomes, groundNormals, uvs, uvIndex, dimsP1);
        uvIndex += 2;
      }
      {
        aos[aoIndex++] = getAo(
          (int)std::round(a[0]),
          (int)std::round(a[1]),
          (int)std::round(a[2]),
          getPotential
        );
        aos[aoIndex++] = getAo(
          (int)std::round(b[0]),
          (int)std::round(b[1]),
          (int)std::round(b[2]),
          getPotential
        );
        aos[aoIndex++] = getAo(
          (int)std::round(c[0]),
          (int)std::round(c[1]),
          (int)std::round(c[2]),
          getPotential
        );
      }
    }
    for (int i = 0; i < 12; i++) {
      std::array<float, 3> &v = edges[i];
      for(int j=0; j<3; ++j) {
        v[j] = (v[j] + shift[j]) * scale[j];
      }
    }
    for(int i=0;f[i]!=-1;i+=3) {
      std::array<float, 3> &a = edges[f[i]];
      std::array<float, 3> &b = edges[f[i+2]];
      std::array<float, 3> &c = edges[f[i+1]];

      if (transparent) {
        a[1] = std::min(a[1], yLimit);
        b[1] = std::min(b[1], yLimit);
        c[1] = std::min(c[1], yLimit);
      }

      positions[positionIndex++] = a[0];
      positions[positionIndex++] = a[1];
      positions[positionIndex++] = a[2];
      positions[positionIndex++] = b[0];
      positions[positionIndex++] = b[1];
      positions[positionIndex++] = b[2];
      positions[positionIndex++] = c[0];
      positions[positionIndex++] = c[1];
      positions[positionIndex++] = c[2];

      Tri tri{
        Vec{a[0], a[1], a[2]},
        Vec{b[0], b[1], b[2]},
        Vec{c[0], c[1], c[2]},
      };
      Vec normal = tri.normal();
      normals[normalIndex++] = normal.x;
      normals[normalIndex++] = normal.y;
      normals[normalIndex++] = normal.z;
      normals[normalIndex++] = normal.x;
      normals[normalIndex++] = normal.y;
      normals[normalIndex++] = normal.z;
      normals[normalIndex++] = normal.x;
      normals[normalIndex++] = normal.y;
      normals[normalIndex++] = normal.z;

      /* barycentrics[barycentricIndex++] = 1;
      barycentrics[barycentricIndex++] = 0;
      barycentrics[barycentricIndex++] = 0;
      barycentrics[barycentricIndex++] = 0;
      barycentrics[barycentricIndex++] = 1;
      barycentrics[barycentricIndex++] = 0;
      barycentrics[barycentricIndex++] = 0;
      barycentrics[barycentricIndex++] = 0;
      barycentrics[barycentricIndex++] = 1; */

      ids[idIndex++] = meshId;
      ids[idIndex++] = meshId;
      ids[idIndex++] = meshId;
    }
  }

  if (!transparent) {
    memset(peeks, 0, 15);
    unsigned char seenPeeks[dimsP1[0] * dimsP1[1] * dimsP1[2]];
    memset(seenPeeks, 0, sizeof(seenPeeks));
    for (int x = 0; x < dimsP1[0]; x++) {
      for (int y = 0; y < dimsP1[0]; y++) {
        _floodFill(x, y, dimsP1[2]-1, (int)PEEK_FACES::FRONT, getPotential, 0, 0, 0, dimsP1[0], dimsP1[1], dimsP1[2], peeks, seenPeeks, dimsP1);
      }
    }
    for (int x = 0; x < dimsP1[0]; x++) {
      for (int y = 0; y < dimsP1[0]; y++) {
        _floodFill(x, y, 0, (int)PEEK_FACES::BACK, getPotential, 0, 0, 0, dimsP1[0], dimsP1[1], dimsP1[2], peeks, seenPeeks, dimsP1);
      }
    }
    for (int z = 0; z < dimsP1[0]; z++) {
      for (int y = 0; y < dimsP1[0]; y++) {
        _floodFill(0, y, z, (int)PEEK_FACES::LEFT, getPotential, 0, 0, 0, dimsP1[0], dimsP1[1], dimsP1[2], peeks, seenPeeks, dimsP1);
      }
    }
    for (int z = 0; z < dimsP1[0]; z++) {
      for (int y = 0; y < dimsP1[0]; y++) {
        _floodFill(dimsP1[0]-1, y, z, (int)PEEK_FACES::RIGHT, getPotential, 0, 0, 0, dimsP1[0], dimsP1[1], dimsP1[2], peeks, seenPeeks, dimsP1);
      }
    }
    for (int x = 0; x < dimsP1[0]; x++) {
      for (int z = 0; z < dimsP1[0]; z++) {
        _floodFill(x, dimsP1[1]-1, z, (int)PEEK_FACES::TOP, getPotential, 0, 0, 0, dimsP1[0], dimsP1[1], dimsP1[2], peeks, seenPeeks, dimsP1);
      }
    }
    for (int x = 0; x < dimsP1[0]; x++) {
      for (int z = 0; z < dimsP1[0]; z++) {
        _floodFill(x, 0, z, (int)PEEK_FACES::BOTTOM, getPotential, 0, 0, 0, dimsP1[0], dimsP1[1], dimsP1[2], peeks, seenPeeks, dimsP1);
      }
    }
  }
}

void marchingCubes2(float meshId, int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, /*float *barycentrics,*/ unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int &positionIndex, unsigned int &normalIndex, unsigned int &uvIndex, /*unsigned int &barycentricIndex,*/ unsigned int &aoIndex, unsigned int &idIndex, unsigned int &skyLightsIndex, unsigned int &torchLightsIndex, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks) {
  positionIndex = 0;
  normalIndex = 0;
  uvIndex = 0;
  // barycentricIndex = 0;
  aoIndex = 0;
  idIndex = 0;
  skyLightsIndex = 0;
  torchLightsIndex = 0;
  numOpaquePositions = 0;
  numTransparentPositions = 0;
  unsigned int lightIndex = 0;

  int dimsP1[3] = {
    dims[0]+1,
    dims[1]+1,
    dims[2]+1,
  };
  int dimsP3[3] = {
    dims[0]+3,
    dims[1]+3,
    dims[2]+3,
  };

  marchingCubesRaw<false>(meshId, dimsP1, [&](int x, int y, int z) -> float {
    int index = getPotentialIndex(x, y, z, dimsP3);
    return potential[index];
  }, [&](int index) -> unsigned char {
    return biomes[index];
  }, heightfield, lightfield, shift, scale, 0.0f, positions, normals, uvs, /*barycentrics,*/ aos, ids, skyLights, torchLights, positionIndex, normalIndex, uvIndex, /*barycentricIndex,*/ aoIndex, idIndex, skyLightsIndex, torchLightsIndex, peeks);
  numOpaquePositions = positionIndex;

  marchingCubesRaw<true>(meshId, dimsP1, [&](int x, int y, int z) -> float {
    int ay = shift[1] + y;
    if (ay < waterLevel) {
      int index = getPotentialIndex(x, y, z, dimsP3);
      return -potential[index];
    } else {
      return -0.5f;
    }
  }, [&](int index) -> unsigned char {
    unsigned char biome = biomes[index];
    switch (biome) {
      case (unsigned char)BIOME::biOcean:
      case (unsigned char)BIOME::biRiver:
        return (unsigned char)BIOME::waterOcean;
      case (unsigned char)BIOME::biFrozenOcean:
      case (unsigned char)BIOME::biFrozenRiver:
        return (unsigned char)BIOME::waterOceanFrozen;
      default: return (unsigned char)BIOME::waterOcean;
    }
  }, heightfield, lightfield, shift, scale, 4.0f, positions, normals, uvs, /*barycentrics,*/ aos, ids, skyLights, torchLights, positionIndex, normalIndex, uvIndex, /*barycentricIndex,*/ aoIndex, idIndex, skyLightsIndex, torchLightsIndex, peeks);
  numTransparentPositions = positionIndex - numOpaquePositions;
}

std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> doMine(Tracker *tracker, float *position, float delta) {
  int x = (int)std::floor(position[0]);
  int y = (int)std::floor(position[1]);
  int z = (int)std::floor(position[2]);
  
  // collect subparcels
  std::vector<std::shared_ptr<Subparcel>> newSubparcels;
  constexpr int radius = 1;
  {
    std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
    newSubparcels.reserve(tracker->subparcels.size());

    for (int dy = -radius - 1; dy <= radius + 1; dy++) {
      const int ay = y + dy;
      for (int dz = -radius - 1; dz <= radius + 1; dz++) {
        const int az = z + dz;
        for (int dx = -radius - 1; dx <= radius + 1; dx++) {
          const int ax = x + dx;
          const int sx = (int)std::floor((float)ax/(float)SUBPARCEL_SIZE);
          const int sy = (int)std::floor((float)ay/(float)SUBPARCEL_SIZE);
          const int sz = (int)std::floor((float)az/(float)SUBPARCEL_SIZE);
          const int index = getSubparcelIndex(sx, sy, sz);
          auto existingSubparcelIter = std::find_if(newSubparcels.begin(), newSubparcels.end(), [&](std::shared_ptr<Subparcel> &subparcel) -> bool {
            return subparcel->coord.index == index;
          });
          if (existingSubparcelIter == newSubparcels.end()) {
            auto oldSubparcelIter = tracker->subparcels.find(index);
            if (oldSubparcelIter != tracker->subparcels.end()) {
              std::shared_ptr<Subparcel> &oldSubparcel = oldSubparcelIter->second;
              if (oldSubparcel->live) {
                newSubparcels.push_back(oldSubparcel);
              } else {
              	return std::pair<bool, std::vector<std::shared_ptr<Subparcel>>>(false, std::vector<std::shared_ptr<Subparcel>>());
                // std::cout << "cannot edit dead index " << ax << " " << ay << " " << az << std::endl;
                // abort();
              }
            } else {
	          	return std::pair<bool, std::vector<std::shared_ptr<Subparcel>>>(false, std::vector<std::shared_ptr<Subparcel>>());
	          }
          }
        }
      }
    }

    for (unsigned int i = 0; i < newSubparcels.size(); i++) {
			std::shared_ptr<Subparcel> &oldSubparcel = newSubparcels[i];
			std::shared_ptr<Subparcel> newSubparcel(oldSubparcel->clone());
			newSubparcel->copyVegetation(*oldSubparcel);
			oldSubparcel->live = false;
			newSubparcels[i] = std::move(newSubparcel);
		}
  }
  // apply edit
  {
    int dimsP3[3] = {
      SUBPARCEL_SIZE_P3,
      SUBPARCEL_SIZE_P3,
      SUBPARCEL_SIZE_P3,
    };
    std::vector<int> minedIndices;
    minedIndices.reserve(32);
    std::function<void(int, int, int, float)> _applyRound = [&](int ax, int ay, int az, float value) -> void {
      minedIndices.clear();
      for (int ddy = -1; ddy <= 1; ddy++) {
        const int ady = ay + ddy;
        for (int ddz = -1; ddz <= 1; ddz++) {
          const int adz = az + ddz;
          for (int ddx = -1; ddx <= 1; ddx++) {
            const int adx = ax + ddx;

            const int sdx = (int)std::floor((float)adx/(float)SUBPARCEL_SIZE);
            const int sdy = (int)std::floor((float)ady/(float)SUBPARCEL_SIZE);
            const int sdz = (int)std::floor((float)adz/(float)SUBPARCEL_SIZE);
            const int index = getSubparcelIndex(sdx, sdy, sdz);
            auto minedIndexIter = std::find(minedIndices.begin(), minedIndices.end(), index);
            if (minedIndexIter == minedIndices.end()) {
              minedIndices.push_back(index);
              
              const int lx = ax - sdx*SUBPARCEL_SIZE;
              const int ly = ay - sdy*SUBPARCEL_SIZE;
              const int lz = az - sdz*SUBPARCEL_SIZE;
              if (
                lx >= 0 && lx < SUBPARCEL_SIZE_P3 &&
                ly >= 0 && ly < SUBPARCEL_SIZE_P3 &&
                lz >= 0 && lz < SUBPARCEL_SIZE_P3
              ) {
                auto subparcelIter = std::find_if(newSubparcels.begin(), newSubparcels.end(), [&](std::shared_ptr<Subparcel> &subparcel) -> bool {
                  return subparcel->coord.index == index;
                });
                if (subparcelIter != newSubparcels.end()) {
                  std::shared_ptr<Subparcel> &subparcel = *subparcelIter;

                  const int potentialIndex = getPotentialIndex(lx, ly, lz, dimsP3);
                  // std::cout << "mine potential " << index << " " << lx << " " << ly << " " << lz << " " << potentialIndex << " " << subparcel->potentials[potentialIndex] << " " << value << std::endl;
                  subparcel->potentials[potentialIndex] += value;
                } else {
                  std::cout << "warning: did not have subparcel: " << sdx << " " << sdy << " " << sdz << std::endl;
                }
              }
            }
          }
        }
      }
    };
    for (int dy = -radius; dy <= radius; dy++) {
      const int ay = y + dy;
      for (int dz = -radius; dz <= radius; dz++) {
        const int az = z + dz;
        for (int dx = -radius; dx <= radius; dx++) {
          const int ax = x + dx;

          const float dist = std::sqrt((float)dx*(float)dx + (float)dy*(float)dy + (float)dz*(float)dz);
          const float maxDistScale = radius;
          const float maxDist = std::sqrt(maxDistScale*maxDistScale + maxDistScale*maxDistScale + maxDistScale*maxDistScale);
          const float distanceDiff = maxDist - dist;
          if (distanceDiff > 0) {
            const float value = (1.0f-dist/maxDist)*delta;
            _applyRound(ax, ay, az, value);
          }
        }
      }
    }
  }
  // re-polygonalize new subparcels
  for (std::shared_ptr<Subparcel> subparcel : newSubparcels) {
    float *landPositions;
    unsigned int numLandPositions;
    FreeEntry *landPositionsEntry;
    FreeEntry *landNormalsEntry;
    FreeEntry *landUvsEntry;
    // FreeEntry *landBarycentricsEntry;
    FreeEntry *landAosEntry;
    FreeEntry *landIdsEntry;
    FreeEntry *landSkyLightsEntry;
    FreeEntry *landTorchLightsEntry;
    {
      float meshId = tracker->meshId;
      int dims[3] = {
        SUBPARCEL_SIZE,
        SUBPARCEL_SIZE,
        SUBPARCEL_SIZE,
      };
      float *potential = subparcel->potentials;
      unsigned char *biomes = subparcel->biomes;
      char *heightfield = subparcel->heightfield;
      unsigned char *lightfield = subparcel->lightfield;
      float shift[3] = {
        (float)subparcel->coord.x*(float)SUBPARCEL_SIZE,
        (float)subparcel->coord.y*(float)SUBPARCEL_SIZE,
        (float)subparcel->coord.z*(float)SUBPARCEL_SIZE,
      };
      float scale[3] = {1, 1, 1};

      ArenaAllocator *positionsAllocator = tracker->landPositionsAllocator;
      ArenaAllocator *normalsAllocator = tracker->landNormalsAllocator;
      ArenaAllocator *uvsAllocator = tracker->landUvsAllocator;
      // ArenaAllocator *barycentricsAllocator = tracker->landBarycentricsAllocator;
      ArenaAllocator *aosAllocator = tracker->landAosAllocator;
      ArenaAllocator *idsAllocator = tracker->landIdsAllocator;
      ArenaAllocator *skyLightsAllocator = tracker->landSkyLightsAllocator;
      ArenaAllocator *torchLightsAllocator = tracker->landTorchLightsAllocator;

      unsigned int numOpaquePositions;
      unsigned int numTransparentPositions;

      constexpr int bufferSize = 1024*1024;
      std::vector<float> positions(bufferSize);
      std::vector<float> normals(bufferSize);
      std::vector<float> uvs(bufferSize);
      // std::vector<float> barycentrics(bufferSize);
      std::vector<unsigned char> aos(bufferSize);
      std::vector<float> ids(bufferSize);
      std::vector<unsigned char> skyLights(bufferSize);
      std::vector<unsigned char> torchLights(bufferSize);
      constexpr unsigned int numPeeks = 15;
      std::vector<unsigned char> peeks(numPeeks);
      unsigned int numPositions;
      unsigned int numNormals;
      unsigned int numUvs;
      // unsigned int numBarycentrics;
      unsigned int numAos;
      unsigned int numIds;
      unsigned int numSkyLights;
      unsigned int numTorchLights;
      marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions.data(), normals.data(), uvs.data(), /*barycentrics.data(),*/ aos.data(), ids.data(), skyLights.data(), torchLights.data(), numPositions, numNormals, numUvs, /*numBarycentrics,*/ numAos, numIds, numSkyLights, numTorchLights, numOpaquePositions, numTransparentPositions, peeks.data());

      subparcel->landPositionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
      if (!subparcel->landPositionsEntry) {
        std::cout << "could not allocate chunk marchingCubes positions" << std::endl;
        abort();
      }
      subparcel->landNormalsEntry = normalsAllocator->alloc(numNormals*sizeof(float));
      if (!subparcel->landNormalsEntry) {
        std::cout << "could not allocate chunk marchingCubes normals" << std::endl;
        abort();
      }
      subparcel->landUvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
      if (!subparcel->landUvsEntry) {
        std::cout << "could not allocate chunk marchingCubes uvs" << std::endl;
        abort();
      }
      /* subparcel->landBarycentricsEntry = barycentricsAllocator->alloc(numBarycentrics*sizeof(float)); // XXX maybe not needed
      if (!subparcel->landBarycentricsEntry) {
        std::cout << "could not allocate chunk marchingCubes barycentrics" << std::endl;
        abort();
      } */
      subparcel->landAosEntry = aosAllocator->alloc(numAos*sizeof(unsigned char));
      if (!subparcel->landAosEntry) {
        std::cout << "could not allocate chunk marchingCubes aos" << std::endl;
        abort();
      }
      subparcel->landIdsEntry = idsAllocator->alloc(numIds*sizeof(float));
      if (!subparcel->landIdsEntry) {
        std::cout << "could not allocate chunk marchingCubes ids" << std::endl;
        abort();
      }
      subparcel->landSkyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
      if (!subparcel->landSkyLightsEntry) {
        std::cout << "could not allocate chunk marchingCubes skyLights" << std::endl;
        abort();
      }
      subparcel->landTorchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
      if (!subparcel->landTorchLightsEntry) {
        std::cout << "could not allocate chunk marchingCubes torchLights" << std::endl;
        abort();
      }

      memcpy(positionsAllocator->data + subparcel->landPositionsEntry->spec.start, positions.data(), numPositions*sizeof(float));
      memcpy(normalsAllocator->data + subparcel->landNormalsEntry->spec.start, normals.data(), numNormals*sizeof(float));
      memcpy(uvsAllocator->data + subparcel->landUvsEntry->spec.start, uvs.data(), numUvs*sizeof(float));
      // memcpy(barycentricsAllocator->data + subparcel->landBarycentricsEntry->spec.start, barycentrics.data(), numBarycentrics*sizeof(float));
      memcpy(aosAllocator->data + subparcel->landAosEntry->spec.start, aos.data(), numAos*sizeof(unsigned char));
      memcpy(idsAllocator->data + subparcel->landIdsEntry->spec.start, ids.data(), numIds*sizeof(float));
      memcpy(skyLightsAllocator->data + subparcel->landSkyLightsEntry->spec.start, skyLights.data(), numSkyLights*sizeof(unsigned char));
      memcpy(torchLightsAllocator->data + subparcel->landTorchLightsEntry->spec.start, torchLights.data(), numTorchLights*sizeof(unsigned char));
      memcpy(subparcel->peeks, peeks.data(), numPeeks*sizeof(unsigned char));

      // groups
      subparcel->landGroups[0].start = subparcel->landPositionsEntry->spec.start/sizeof(float)/3;
      subparcel->landGroups[0].count = numOpaquePositions/3;
      subparcel->landGroups[0].materialIndex = 0;
      subparcel->landGroups[1].start = subparcel->landGroups[0].start + subparcel->landGroups[0].count;
      subparcel->landGroups[1].count = numTransparentPositions/3;
      subparcel->landGroups[1].materialIndex = 1;

      // latch
      landPositions = (float *)(positionsAllocator->data + subparcel->landPositionsEntry->spec.start);
      numLandPositions = numOpaquePositions;
    }
    doLandPhysics(tracker, subparcel.get(), landPositions, numLandPositions);
  }

  return std::pair<bool, std::vector<std::shared_ptr<Subparcel>>>(true, std::move(newSubparcels));
  
  /* const _mine = async (mineSpecs, explodePosition) => {
  const slabs = mineSpecs.map(spec => currentChunkMesh.getSlab(spec.x, spec.y, spec.z));
  const specs = await chunkWorker.requestMine(currentChunkMesh.meshId, mineSpecs);
  for (let i = 0; i < slabs.length; i++) {
    const slab = slabs[i];
    const spec = specs[i];
    currentChunkMesh.updateGeometry(slab, spec);
  }
  for (let i = 0; i < slabs.length; i++) {
    if (slab.physxGroupSet) {
      geometryWorker.unregisterGroupSet(culler, slab.physxGroupSet);
      slab.physxGroupSet = 0;
    }
    const peeks = new Uint8Array(currentChunkMesh.geometry.peeks.buffer, currentChunkMesh.geometry.peeks.byteOffset + slab.spec.peeksStart, slab.spec.peeksCount);
    slab.physxGroupSet = geometryWorker.registerGroupSet(culler, slab.x, slab.y, slab.z, slabRadius, peeks, slab.groupSet.groups);
  }
  const neededSpecs = specs.filter(spec => spec.numOpaquePositions > 0);
  if (neededSpecs.length > 0) {
    const bakeSpecs = neededSpecs.map(spec => {
      const {positions, numOpaquePositions, x, y, z} = spec;
      return numOpaquePositions > 0 ? {
        positions,
        numOpaquePositions,
        x,
        y,
        z,
      } : null;
    });
    const result = await geometryWorker.requestBakeGeometries(bakeSpecs.map(spec => ({
      positions: spec.positions,
      count: spec.numOpaquePositions,
    })));
    for (let i = 0; i < result.physicsGeometryBuffers.length; i++) {
      const physxGeometry = result.physicsGeometryBuffers[i];
      const {x, y, z} = bakeSpecs[i];
      // const stat = bakeStats[i];
      const slab = currentChunkMesh.getSlab(x, y, z);
      if (slab.physxGeometry) {
        geometryWorker.unregisterGeometry(slab.physxGeometry);
        slab.physxGeometry = 0;
      }
      slab.physxGeometry = geometryWorker.registerBakedGeometry(currentChunkMesh.meshId, physxGeometry, x, y, z);
    }
  }
  if (specs.length > 0 && explodePosition) {
    for (let i = 0; i < 3; i++) {
      const pxMesh = new THREE.Mesh(tetrehedronGeometry, currentChunkMesh.material[0]);
      currentChunkMesh.getWorldQuaternion(localQuaternion2).inverse();
      pxMesh.position.copy(explodePosition)
        .add(localVector2.set((-1+Math.random()*2)*0.2, 0.2, (-1+Math.random()*2)*0.2).applyQuaternion(localQuaternion2));
      pxMesh.velocity = new THREE.Vector3((-1+Math.random()*2)*0.5, Math.random()*3, (-1+Math.random()*2)*0.5)
        .applyQuaternion(localQuaternion2);
      pxMesh.angularVelocity = new THREE.Vector3((-1+Math.random()*2)*Math.PI*2*0.01, (-1+Math.random()*2)*Math.PI*2*0.01, (-1+Math.random()*2)*Math.PI*2*0.01);
      pxMesh.isBuildMesh = true;
      const startTime = Date.now();
      const endTime = startTime + 3000;
      pxMesh.update = () => Date.now() < endTime;
      currentChunkMesh.add(pxMesh);
      pxMeshes.push(pxMesh);
    }
  }
};
const _applyMineSpec = (p, radius, key, dim, getIndex, delta) => {
  const mineSpecs = [];
  const _applyRound = (ax, ay, az, value) => {
    const mineSpecsRound = [];
    for (let ddy = -1; ddy <= 1; ddy++) {
      const ady = ay + ddy;
      for (let ddz = -1; ddz <= 1; ddz++) {
        const adz = az + ddz;
        for (let ddx = -1; ddx <= 1; ddx++) {
          const adx = ax + ddx;

          const sdx = Math.floor(adx/currentChunkMesh.subparcelSize);
          const sdy = Math.floor(ady/currentChunkMesh.subparcelSize);
          const sdz = Math.floor(adz/currentChunkMesh.subparcelSize);
          const lx = ax - sdx*currentChunkMesh.subparcelSize;
          const ly = ay - sdy*currentChunkMesh.subparcelSize;
          const lz = az - sdz*currentChunkMesh.subparcelSize;

          if (
            lx >= 0 && lx < dim &&
            ly >= 0 && ly < dim &&
            lz >= 0 && lz < dim
          ) {
            const index = planet.getSubparcelIndex(sdx, sdy, sdz);
            if (!mineSpecsRound.some(mineSpec => mineSpec.index === index)) {
              planet.editSubparcel(sdx, sdy, sdz, subparcel => {
                const potentialIndex = getIndex(lx, ly, lz);
                subparcel[key][potentialIndex] += value;

                const slab = currentChunkMesh.getSlab(sdx, sdy, sdz);

                const mineSpec = {
                  x: sdx,
                  y: sdy,
                  z: sdz,
                  index,
                  potentials: subparcel.potentials,
                  biomes: subparcel.biomes,
                  heightfield: subparcel.heightfield,
                  lightfield: subparcel.lightfield,
                  position: slab.position,
                  normal: slab.normal,
                  uv: slab.uv,
                  barycentric: slab.barycentric,
                  ao: slab.ao,
                  id: slab.id,
                  skyLight: slab.skyLight,
                  torchLight: slab.torchLight,
                  peeks: slab.peeks,
                };
                mineSpecsRound.push(mineSpec);
                if (!mineSpecs.some(mineSpec => mineSpec.index === index)) {
                  mineSpecs.push(mineSpec);
                }
              });
            }
          }
        }
      }
    }
  };

  const {x, y, z} = p;
  for (let dy = -radius; dy <= radius; dy++) {
    const ay = y + dy;
    for (let dz = -radius; dz <= radius; dz++) {
      const az = z + dz;
      for (let dx = -radius; dx <= radius; dx++) {
        const ax = x + dx;

        const dist = Math.sqrt(dx*dx + dy*dy + dz*dz);
        const maxDistScale = radius;
        const maxDist = Math.sqrt(maxDistScale*maxDistScale + maxDistScale*maxDistScale + maxDistScale*maxDistScale);
        const distanceDiff = maxDist - dist;
        if (distanceDiff > 0) {
          const value = (1-dist/maxDist)*delta;
          _applyRound(ax, ay, az, value);
        }
      }
    }
  }
  return mineSpecs;
};
const _applyPotentialDelta = async (position, delta) => {
  localVector2.copy(position)
    .applyMatrix4(localMatrix.getInverse(currentChunkMesh.matrixWorld));
  const applyPosition = localVector2.clone();
  localVector2.x = Math.floor(localVector2.x);
  localVector2.y = Math.floor(localVector2.y);
  localVector2.z = Math.floor(localVector2.z);

  const mineSpecs = _applyMineSpec(localVector2, 1, 'potentials', SUBPARCEL_SIZE_P3, planet.getPotentialIndex, delta);
  await _mine(mineSpecs, delta < 0 ? applyPosition : null);
}; */
}
std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> doLight(Tracker *tracker, float *position, float delta) {
  int x = (int)position[0];
  int y = (int)position[1];
  int z = (int)position[2];
  constexpr int radius = 1;
  int dimsP1[3] = {
    SUBPARCEL_SIZE_P1,
    SUBPARCEL_SIZE_P1,
    SUBPARCEL_SIZE_P1,
  };

  std::vector<std::shared_ptr<Subparcel>> mineSpecs;
  std::function<void(int, int, int, float)> _applyRound = [&](int ax, int ay, int az, float value) -> void {
    std::vector<int> mineSpecsRound;
    for (int ddy = -1; ddy <= 1; ddy++) {
      const int ady = ay + ddy;
      for (int ddz = -1; ddz <= 1; ddz++) {
        const int adz = az + ddz;
        for (int ddx = -1; ddx <= 1; ddx++) {
          const int adx = ax + ddx;

          const int sdx = (int)std::floor((float)adx/(float)SUBPARCEL_SIZE);
          const int sdy = (int)std::floor((float)ady/(float)SUBPARCEL_SIZE);
          const int sdz = (int)std::floor((float)adz/(float)SUBPARCEL_SIZE);
          const int lx = ax - sdx*SUBPARCEL_SIZE;
          const int ly = ay - sdy*SUBPARCEL_SIZE;
          const int lz = az - sdz*SUBPARCEL_SIZE;

          if (
            lx >= 0 && lx < SUBPARCEL_SIZE_P1 &&
            ly >= 0 && ly < SUBPARCEL_SIZE_P1 &&
            lz >= 0 && lz < SUBPARCEL_SIZE_P1
          ) {
            const int index = getSubparcelIndex(sdx, sdy, sdz);
            if (std::find(mineSpecsRound.begin(), mineSpecsRound.end(), index) == mineSpecsRound.end()) {
              auto subparcelIter = tracker->subparcels.find(index);
              if (subparcelIter != tracker->subparcels.end()) {
                std::shared_ptr<Subparcel> subparcel = subparcelIter->second;

                const int potentialIndex = getPotentialIndex(lx, ly, lz, dimsP1);
                subparcel->lightfield[potentialIndex] += value;

                mineSpecsRound.push_back(index);
                if (std::find(mineSpecs.begin(), mineSpecs.end(), subparcel) == mineSpecs.end()) {
                  mineSpecs.push_back(subparcel);
                }
              }
            }
          }
        }
      }
    }
  };

  {
    std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);

    for (int dy = -radius; dy <= radius; dy++) {
      const int ay = y + dy;
      for (int dz = -radius; dz <= radius; dz++) {
        const int az = z + dz;
        for (int dx = -radius; dx <= radius; dx++) {
          const int ax = x + dx;

          const float dist = std::sqrt((float)dx*(float)dx + (float)dy*(float)dy + (float)dz*(float)dz);
          const float maxDistScale = radius;
          const float maxDist = std::sqrt(maxDistScale*maxDistScale + maxDistScale*maxDistScale + maxDistScale*maxDistScale);
          const float distanceDiff = maxDist - dist;
          if (distanceDiff > 0) {
            const float value = (1.0f-dist/maxDist)*delta;
            _applyRound(ax, ay, az, value);
          }
        }
      }
    }
  }

  return std::pair<bool, std::vector<std::shared_ptr<Subparcel>>>(true, std::move(mineSpecs));
}