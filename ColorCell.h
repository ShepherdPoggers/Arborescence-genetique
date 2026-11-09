#pragma once
#include <iostream>
#ifndef ARBORESCENCE_GENETIQUE_COLORCELL_H
#define ARBORESCENCE_GENETIQUE_COLORCELL_H

class ColorCell{
private:
    int rouge;
    int vert;
    int bleu;

public:
    ColorCell(int rouge, int vert, int bleu);
    ColorCell();
    static ColorCell additionCellule(ColorCell c1, ColorCell c2);
    void afficher();
};


#endif
