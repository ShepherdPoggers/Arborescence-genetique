#include <iostream>
#include "ColorCell.h"
#include "ChromoCell.h"
using namespace std;

int main()
{
    //TEST
    ChromoCell c1("IJ", "KL", "AC", "BD");
    ChromoCell c2("AB", "CD", "EF", "GH");
    ChromoCell c3 = ChromoCell::additionCellule(c1, c2);
    c3.afficher();

    return 0;
}
