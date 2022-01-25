#include <vector>
#include "march.h"

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

const int width = 10;
const int height = 10;
const int depth = 10;
const float potentialClearValue = -0.2;
const float potentialSetValue = 0.7;
inline int _getPotentialIndex(int x, int y, int z) {
  return x + y*(width+1)*(depth+1) + z*(width+1);
}
inline std::array<float,3> add(const std::array<float,3> &a, const std::array<float,3> &b) {
  return std::array<float,3>{a[0]+b[0], a[1]+b[1], a[2]+b[2]};
}
inline std::array<float,3> sub(const std::array<float,3> &a, const std::array<float,3> &b) {
  return std::array<float,3>{a[0]-b[0], a[1]-b[1], a[2]-b[2]};
}
inline std::array<int,3> add(const std::array<int,3> &a, const std::array<int,3> &b) {
  return std::array<int,3>{a[0]+b[0], a[1]+b[1], a[2]+b[2]};
}
inline std::array<int,3> sub(const std::array<int,3> &a, const std::array<int,3> &b) {
  return std::array<int,3>{a[0]-b[0], a[1]-b[1], a[2]-b[2]};
}
inline std::array<float,3> multiplyScalar(const std::array<float,3> &a, const float &c) {
  return std::array<float,3>{a[0]*c, a[1]*c, a[2]*c};
}
inline std::array<float,3> divideScalar(const std::array<float,3> &a, const float &c) {
  return std::array<float,3>{a[0]/c, a[1]/c, a[2]/c};
}
std::pair<std::array<int,3>, std::vector<float> *> _getChunkAt(float x, float y, float z, int *chunkCoords, unsigned int numChunkCoords, std::vector<std::vector<float>> &workingPotentialsArray) {
  for (unsigned int i = 0; i < numChunkCoords; i++) {
    std::array<float,3> chunkPositionFloat = {
      (float)chunkCoords[i*3],
      (float)chunkCoords[i*3+1],
      (float)chunkCoords[i*3+2]
    };

    const float dx = x - chunkPositionFloat[0];
    const float dy = y - chunkPositionFloat[1];
    const float dz = z - chunkPositionFloat[2];
    if (dx >= 0 && dx < 1 && dy >= 0 && dy < 1 && dz >= 0 && dz < 1) {
      std::array<int,3> chunkPosition = {
        chunkCoords[i*3],
        chunkCoords[i*3+1],
        chunkCoords[i*3+2]
      };
      return std::pair<std::array<int,3>, std::vector<float> *>(std::move(chunkPosition), &workingPotentialsArray[i]);
    }
  }
  return std::pair<std::array<int,3>, std::vector<float> *>(std::array<int,3>{0, 0, 0}, nullptr);
}
int _getChunkIndexAt(int x, int y, int z, int *chunkCoords, unsigned int numChunkCoords) {
  for (unsigned int i = 0; i < numChunkCoords; i++) {
    if (chunkCoords[i*3] == x && chunkCoords[i*3+1] == y && chunkCoords[i*3+2] == z) {
      return i;
    }
  }
  return -1;
}

void smoothedPotentials(int *chunkCoords, unsigned int numChunkCoords, float *colorTargetCoordBuf, int colorTargetSize, float voxelSize, float *potentialsBuffer) {
  std::vector<std::vector<float>> workingPotentialsArray;
  workingPotentialsArray.reserve(numChunkCoords);
  std::vector<int> dirtyWorkingPotentials;
  dirtyWorkingPotentials.reserve(numChunkCoords);
  const unsigned int potentialsBlockSize = (width+1)*(height+1)*(depth+1);

  for (unsigned int i = 0; i < numChunkCoords; i++) {
    std::array<int,3> chunkPosition = {
      chunkCoords[i*3],
      chunkCoords[i*3+1],
      chunkCoords[i*3+2]
    };

    workingPotentialsArray.push_back(std::vector<float>(potentialsBlockSize));
    std::vector<float> &workingPotentials = workingPotentialsArray.back();
    dirtyWorkingPotentials.push_back(false);
    int &dirty = dirtyWorkingPotentials.back();
    dirty = 0;

    std::fill(workingPotentials.begin(), workingPotentials.end(), potentialClearValue);

    int index = 0;
    for (int x = 0; x < colorTargetSize; x++) {
      for (int y = 0; y < colorTargetSize; y++) {
        std::array<float,3> localVector = {
          colorTargetCoordBuf[index],
          colorTargetCoordBuf[index+1],
          colorTargetCoordBuf[index+2]
        };

        std::array<float,3> chunkPositionFloat = {
          (float)chunkPosition[0],
          (float)chunkPosition[1],
          (float)chunkPosition[2]
        };
        localVector = sub(localVector, chunkPositionFloat);
        localVector = divideScalar(localVector, voxelSize);
        localVector = add(localVector, std::array<float,3>{0.5, 0.5, 0.5});

        if (
          localVector[0] >= 0 && localVector[0] < (width+1) &&
          localVector[1] >= 0 && localVector[1] < (height+1) &&
          localVector[2] >= 0 && localVector[2] < (depth+1)
        ) {
          workingPotentials[_getPotentialIndex(
            (int)std::floor(localVector[0]),
            (int)std::floor(localVector[1]),
            (int)std::floor(localVector[2])
          )] = potentialSetValue;
          dirty = 1;
        }
        index += 3;
      }
    }
  }

  for (unsigned int i = 0; i < numChunkCoords; i++) {
    float *potentials = &potentialsBuffer[i*potentialsBlockSize];

    if (dirtyWorkingPotentials[i]) {
      std::array<int,3> chunkPosition = {
        chunkCoords[i*3],
        chunkCoords[i*3+1],
        chunkCoords[i*3+2]
      };
      std::vector<float> &workingPotentials = workingPotentialsArray[i];

      int index = 0;
      for (int y = 0; y < height+1; y++) {
        for (int z = 0; z < depth+1; z++) {
          for (int x = 0; x < width+1; x++) {
            float sum = 0;
            for (int dy = -1; dy <= 1; dy++) {
              const int ay = y + dy;
              for (int dz = -1; dz <= 1; dz++) {
                const int az = z + dz;
                for (int dx = -1; dx <= 1; dx++) {
                  const int ax = x + dx;
                  if (ax >= 0 && ax < width && ay >= 0 && ay < height && az >= 0 && az < depth) {
                    sum += workingPotentials[_getPotentialIndex(ax, ay, az)];
                  } else {
                    std::pair<std::array<int,3>, std::vector<float> *> chunk = _getChunkAt(
                      (float)(chunkPosition[0]) + ((float)ax)*voxelSize,
                      (float)(chunkPosition[1]) + ((float)ay)*voxelSize,
                      (float)(chunkPosition[2]) + ((float)az)*voxelSize,
                      chunkCoords,
                      numChunkCoords,
                      workingPotentialsArray
                    );
                    if (chunk.second) {
                      std::array<int,3> &otherChunkPosition = chunk.first;
                      std::vector<float> &workingPotentials = *(chunk.second);

                      std::array<int,3> chunkOffset = {
                        chunkPosition[0],
                        chunkPosition[1],
                        chunkPosition[2]
                      };
                      chunkOffset = sub(chunkOffset, otherChunkPosition);
                      const int lax = ax + chunkOffset[0]*width;
                      const int lay = ay + chunkOffset[1]*height;
                      const int laz = az + chunkOffset[2]*depth;
                      sum += workingPotentials[_getPotentialIndex(lax, lay, laz)];
                    } else {
                      sum += potentialClearValue;
                    }
                  }
                }
              }
            }
            potentials[index++] = sum/(3*3*3);
          }
        }
      }
    } else {
      for (unsigned int i = 0; i < potentialsBlockSize; i++) {
        potentials[i] = potentialClearValue;
      }
    }
  }
}
float* marchingCubes(int dims[3], float *potential, float shift[3], float scale[3]) {
// void marchingCubes(int dims[3], float *potential, uint8_t *brush, float shift[3], float scale[3], float *positions, float *colors, unsigned int *faces, unsigned int &positionIndex, unsigned int &colorIndex, unsigned int &faceIndex) {
  uint32_t positionIndex = 0;
  // colorIndex = 0;
  uint32_t faceIndex = 0;

  std::vector<float> positions;
  std::vector<uint32_t> faces;

  int n = 0;
  float grid[8] = {0};
  int edges[12] = {0};
  int x[3] = {0};

  //March over the volume
  for(x[2]=0; x[2]<dims[2]-1; ++x[2], n+=dims[0])
  for(x[1]=0; x[1]<dims[1]-1; ++x[1], ++n)
  for(x[0]=0; x[0]<dims[0]-1; ++x[0], ++n) {
    //For each cell, compute cube mask
    int cube_index = 0;
    for(int i=0; i<8; ++i) {
      int *v = cubeVerts[i];
      int potentialIndex = (x[0]+v[0]) +
        (((x[2]+v[2])) * dims[0]) +
        ((x[1]+v[1]) * dims[0] * dims[1]);
      float s = potential[potentialIndex];
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
      edges[i] = positionIndex / 3;
      int *e = edgeIndex[i];
      int *p0 = cubeVerts[e[0]];
      int *p1 = cubeVerts[e[1]];
      float a = grid[e[0]];
      float b = grid[e[1]];
      float d = a - b;
      float t = a / d;
      for(int j=0; j<3; ++j) {
        // positions[positionIndex + j] = (((x[j] + p0[j]) + t * (p1[j] - p0[j])) + shift[j]) * scale[j];
        positions.push_back((((x[j] + p0[j]) + t * (p1[j] - p0[j])) + shift[j]) * scale[j]);
      }

      // int brushIndex = (x[0]) +
      //   (((x[2])) * dims[0]) +
      //   ((x[1]) * dims[0] * dims[1]);
      // colors[colorIndex] = (float)brush[brushIndex*3] / 255.0f;
      // colors[colorIndex+1] = (float)brush[brushIndex*3+1] / 255.0f;
      // colors[colorIndex+2] = (float)brush[brushIndex*3+2] / 255.0f;

      positionIndex += 3;
      // colorIndex += 3;
    }
    //Add faces
    int *f = triTable[cube_index];
    for(int i=0;f[i]!=-1;i+=3) {
      // faces[faceIndex++] = edges[f[i]];
      // faces[faceIndex++] = edges[f[i+1]];
      // faces[faceIndex++] = edges[f[i+2]];
	  	faces.push_back(edges[f[i]]);
      faces.push_back(edges[f[i+1]]);
      faces.push_back(edges[f[i+2]]);
      faceIndex += 3;
    }
  }

  float *outputBuffer = (float*)malloc((2 + positions.size() + faces.size()) * 4);

  memcpy(outputBuffer, &positionIndex, 4);
  memcpy(outputBuffer + 1, &faceIndex, 4);
  memcpy(outputBuffer + 2, &positions.front(), positions.size() * 4);
  memcpy(outputBuffer + 2 + positions.size(), &faces.front(), faces.size() * 4);

  return outputBuffer;
}

void computeGeometry(int *chunkCoords, unsigned int numChunkCoords, float *colorTargetCoordBuf, int colorTargetSize, float voxelSize, float marchCubesTexSize, float marchCubesTexSquares, float marchCubesTexTriangleSize, float *potentialsBuffer, float *positionsBuffer, float *barycentricsBuffer, float *uvsBuffer, float *uvs2Buffer, unsigned int *positionIndexBuffer, unsigned int *barycentricIndexBuffer, unsigned int *uvIndexBuffer, unsigned int *uvIndex2Buffer) {
  // working potentials
  std::vector<std::vector<float>> workingPotentialsArray;
  workingPotentialsArray.reserve(numChunkCoords);
  std::vector<int> dirtyWorkingPotentials(numChunkCoords);
  const unsigned int potentialsBlockSize = (width+1)*(height+1)*(depth+1);

  for (unsigned int i = 0; i < numChunkCoords; i++) {
    std::array<int,3> chunkPosition = {
      chunkCoords[i*3],
      chunkCoords[i*3+1],
      chunkCoords[i*3+2]
    };

    workingPotentialsArray.push_back(std::vector<float>(potentialsBlockSize));
    std::vector<float> &workingPotentials = workingPotentialsArray.back();
    // int &dirty = dirtyWorkingPotentials[i];

    std::fill(workingPotentials.begin(), workingPotentials.end(), potentialClearValue);

    int index = 0;
    for (int x = 0; x < colorTargetSize; x++) {
      for (int y = 0; y < colorTargetSize; y++) {
        std::array<float,3> localVector = {
          colorTargetCoordBuf[index],
          colorTargetCoordBuf[index+1],
          colorTargetCoordBuf[index+2]
        };

        std::array<float,3> chunkPositionFloat = {
          (float)chunkPosition[0],
          (float)chunkPosition[1],
          (float)chunkPosition[2]
        };
        localVector = sub(localVector, chunkPositionFloat);
        localVector = divideScalar(localVector, voxelSize);
        localVector = add(localVector, std::array<float,3>{0.5, 0.5, 0.5});

        if (
          localVector[0] >= 0 && localVector[0] < (width+1) &&
          localVector[1] >= 0 && localVector[1] < (height+1) &&
          localVector[2] >= 0 && localVector[2] < (depth+1)
        ) {
          std::array<int,3> point = {
            (int)std::floor(localVector[0]),
            (int)std::floor(localVector[1]),
            (int)std::floor(localVector[2])
          };
          workingPotentials[_getPotentialIndex(point[0], point[1], point[2])] = potentialSetValue;
          // dirty = 1;

          int minDx = point[0] <= 1 ? -1 : 0;
          int maxDx = point[0] >= (width-1) ? 1 : 0;
          int minDy = point[1] <= 1 ? -1 : 0;
          int maxDy = point[1] >= (height-1) ? 1 : 0;
          int minDz = point[2] <= 1 ? -1 : 0;
          int maxDz = point[2] >= (depth-1) ? 1 : 0;
          for (int dz = minDz; dz <= maxDz; dz++) {
            for (int dy = minDy; dy <= maxDy; dy++) {
              for (int dx = minDx; dx <= maxDx; dx++) {
                int otherChunkIndex = _getChunkIndexAt(chunkPosition[0]+dx, chunkPosition[1]+dy, chunkPosition[2]+dz, chunkCoords, numChunkCoords);
                if (otherChunkIndex != -1) {
                  dirtyWorkingPotentials[otherChunkIndex] = 1;
                }
              }
            }
          }
        }
        index += 3;
      }
    }
  }

  // potentials
  for (unsigned int i = 0; i < numChunkCoords; i++) {
    float *potentials = &potentialsBuffer[i*potentialsBlockSize];

    if (dirtyWorkingPotentials[i]) {
      std::array<int,3> chunkPosition = {
        chunkCoords[i*3],
        chunkCoords[i*3+1],
        chunkCoords[i*3+2]
      };
      std::vector<float> &workingPotentials = workingPotentialsArray[i];

      int index = 0;
      for (int y = 0; y < height+1; y++) {
        for (int z = 0; z < depth+1; z++) {
          for (int x = 0; x < width+1; x++) {
            float sum = 0;
            for (int dy = -1; dy <= 1; dy++) {
              const int ay = y + dy;
              for (int dz = -1; dz <= 1; dz++) {
                const int az = z + dz;
                for (int dx = -1; dx <= 1; dx++) {
                  const int ax = x + dx;
                  if (ax >= 0 && ax < width && ay >= 0 && ay < height && az >= 0 && az < depth) {
                    sum += workingPotentials[_getPotentialIndex(ax, ay, az)];
                  } else {
                    std::pair<std::array<int,3>, std::vector<float> *> chunk = _getChunkAt(
                      (float)(chunkPosition[0]) + ((float)ax)*voxelSize,
                      (float)(chunkPosition[1]) + ((float)ay)*voxelSize,
                      (float)(chunkPosition[2]) + ((float)az)*voxelSize,
                      chunkCoords,
                      numChunkCoords,
                      workingPotentialsArray
                    );
                    if (chunk.second) {
                      std::array<int,3> &otherChunkPosition = chunk.first;
                      std::vector<float> &workingPotentials = *(chunk.second);

                      std::array<int,3> chunkOffset = {
                        chunkPosition[0],
                        chunkPosition[1],
                        chunkPosition[2]
                      };
                      chunkOffset = sub(chunkOffset, otherChunkPosition);
                      const int lax = ax + chunkOffset[0]*width;
                      const int lay = ay + chunkOffset[1]*height;
                      const int laz = az + chunkOffset[2]*depth;
                      sum += workingPotentials[_getPotentialIndex(lax, lay, laz)];
                    } else {
                      sum += potentialClearValue;
                    }
                  }
                }
              }
            }
            potentials[index++] = sum/(3*3*3);
          }
        }
      }
    } else {
      for (unsigned int i = 0; i < potentialsBlockSize; i++) {
        potentials[i] = potentialClearValue;
      }
    }
  }

  // marching cubes
  unsigned int positionIndex = 0;
  unsigned int barycentricIndex = 0;
  unsigned int uvIndex = 0;
  unsigned int uvIndex2 = 0;
  for (unsigned int i = 0; i < numChunkCoords; i++) {
    if (dirtyWorkingPotentials[i]) {
      float *potentials = &potentialsBuffer[i*potentialsBlockSize];
      float *positions = &positionsBuffer[positionIndex];
      float *barycentrics = &barycentricsBuffer[barycentricIndex];
      float *uvs = &uvsBuffer[uvIndex];
      float *uvs2 = &uvs2Buffer[uvIndex2];

      unsigned int faceIndex = 0;

      int n = 0;
      float grid[8] = {0};
      std::array<std::array<float, 3>, 12> edges;
      int x[3] = {0};

      //March over the volume
      int dims[3] = {width+1, depth+1, height+1};
      for(x[2]=0; x[2]<dims[2]-1; ++x[2], n+=dims[0])
      for(x[1]=0; x[1]<dims[1]-1; ++x[1], ++n)
      for(x[0]=0; x[0]<dims[0]-1; ++x[0], ++n) {
        //For each cell, compute cube mask
        int cube_index = 0;
        for(int i=0; i<8; ++i) {
          int *v = cubeVerts[i];
          float s = potentials[
            (x[0]+v[0]) +
            (((x[2]+v[2])) * dims[0]) +
            ((x[1]+v[1]) * dims[0] * dims[1])
          ];

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
            v[j] = ((x[j] + p0[j]) + t * (p1[j] - p0[j])); // + shift[j];
          }
        }
        //Add faces
        int *f = triTable[cube_index];
        for(int i=0;f[i]!=-1;i+=3) {
          std::array<float, 3> &a = edges[f[i]];
          std::array<float, 3> &b = edges[f[i+2]];
          std::array<float, 3> &c = edges[f[i+1]];

          const int baseIndex = faceIndex*3;
          positions[baseIndex] = a[0];
          positions[baseIndex+1] = a[1];
          positions[baseIndex+2] = a[2];
          positions[baseIndex+3] = b[0];
          positions[baseIndex+4] = b[1];
          positions[baseIndex+5] = b[2];
          positions[baseIndex+6] = c[0];
          positions[baseIndex+7] = c[1];
          positions[baseIndex+8] = c[2];

          barycentrics[baseIndex] = 1;
          barycentrics[baseIndex+1] = 0;
          barycentrics[baseIndex+2] = 0;
          barycentrics[baseIndex+3] = 0;
          barycentrics[baseIndex+4] = 1;
          barycentrics[baseIndex+5] = 0;
          barycentrics[baseIndex+6] = 0;
          barycentrics[baseIndex+7] = 0;
          barycentrics[baseIndex+8] = 1;

          const int gridI = faceIndex/3;
          const int baseIndex2 = faceIndex*2;
          if ((gridI%2) == 0) {
            const float cx = std::fmod(((float)gridI/2.0f), (marchCubesTexSquares));
            const float cy = std::floor((float)gridI/2.0f/(marchCubesTexSquares));
            uvs[baseIndex2] = cx*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+1] = cy*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+2] = (cx+1.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+3] = (cy+1.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+4] = cx*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+5] = (cy+1.0f)*marchCubesTexTriangleSize/marchCubesTexSize;

            uvs2[baseIndex2] = (cx+1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+1] = (cy+1.0f/marchCubesTexTriangleSize*2.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+2] = (cx+1.0f-1.0f/marchCubesTexTriangleSize*2.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+3] = (cy+1.0f-1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+4] = (cx+1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+5] = (cy+1.0f-1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
          } else {
            const float cx = std::fmod((((float)gridI-1.0f)/2.0f), (marchCubesTexSquares));
            const float cy = std::floor(((float)gridI-1.0f)/2.0f/(marchCubesTexSquares));
            uvs[baseIndex2] = cx*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+1] = cy*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+2] = (cx+1.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+3] = cy*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+4] = (cx+1.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs[baseIndex2+5] = (cy+1.0f)*marchCubesTexTriangleSize/marchCubesTexSize;

            uvs2[baseIndex2] = (cx+1.0f/marchCubesTexTriangleSize*2.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+1] = (cy+1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+2] = (cx+1.0f-1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+3] = (cy+1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+4] = (cx+1.0f-1.0f/marchCubesTexTriangleSize)*marchCubesTexTriangleSize/marchCubesTexSize;
            uvs2[baseIndex2+5] = (cy+1.0f-1.0f/marchCubesTexTriangleSize*2.0f)*marchCubesTexTriangleSize/marchCubesTexSize;
          }

          faceIndex += 3;
        }
      }

      positionIndexBuffer[i] = faceIndex*3;
      barycentricIndexBuffer[i] = faceIndex*3;
      uvIndexBuffer[i] = faceIndex*2;
      uvIndex2Buffer[i] = faceIndex*2;

      positionIndex += positionIndexBuffer[i];
      barycentricIndex += barycentricIndexBuffer[i];
      uvIndex += uvIndexBuffer[i];
      uvIndex2 += uvIndex2Buffer[i];
    } else {
      positionIndexBuffer[i] = 0;
      barycentricIndexBuffer[i] = 0;
      uvIndexBuffer[i] = 0;
      uvIndex2Buffer[i] = 0;
    }
  }
}

void collide(float *positions, unsigned int numPositions, float origin[3], float direction[3], float *collision, unsigned int *collisionIndex) {
  collision[0] = std::numeric_limits<float>::quiet_NaN();
  collision[1] = std::numeric_limits<float>::quiet_NaN();
  collision[2] = std::numeric_limits<float>::quiet_NaN();

  float closestDistance = std::numeric_limits<float>::infinity();

  const Ray ray(Vec(origin[0], origin[1], origin[2]), Vec(direction[0], direction[1], direction[2]));
  for (unsigned int i = 0; i < numPositions; i += 9) {
    Tri triangle(
      Vec(positions[i], positions[i+1], positions[i+2]),
      Vec(positions[i+3], positions[i+4], positions[i+5]),
      Vec(positions[i+6], positions[i+7], positions[i+8])
    );
    Vec intersectionVector;
    if (ray.intersectTriangle(triangle, intersectionVector)) {
      const float distance = (intersectionVector - ray.origin).magnitude();
      if (distance < closestDistance) {
        collision[0] = intersectionVector.x;
        collision[1] = intersectionVector.y;
        collision[2] = intersectionVector.z;
        closestDistance = distance;
        *collisionIndex = i;
      }
    }
  }
}
