#include <iostream>
#include "ColorCell.h"
using namespace std;

int main()
{
    ColorCell c1(255, 0, 0);
    ColorCell c2(0, 0, 255);

    ColorCell c3 = ColorCell::additionCellule(c1, c2);

    c3.afficher();
    return 0;
}
