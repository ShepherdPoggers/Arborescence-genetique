#pragma once
#include <iostream>
#include "cell.h"
#include <string>
#ifndef ARBORESCENCE_GENETIQUE_COLORCELL_H
#define ARBORESCENCE_GENETIQUE_COLORCELL_H

class ColorCell : public Cell{
private:
    int rouge;
    int vert;
    int bleu;

public:
    ColorCell(std::string unNom, int rouge, int vert, int bleu);
    ColorCell* operator+(const Cell& autre) override;
    void afficher();
    void afficher(std::ostream& os) const override;
};


#endif
