#pragma once
#include <iostream>
#include <string>
#include "cell.h"

#ifndef ARBORESCENCE_GENETIQUE_CHROMOCELL_H
#define ARBORESCENCE_GENETIQUE_CHROMOCELL_H

class ChromoCell : public Cell
{
private:
    std::string paire1;
    std::string paire2;
    std::string paire3;
    std::string paire4;

public:
    ChromoCell();
    ChromoCell(std::string unNom, std::string paire1, std::string paire2,
               std::string paire3, std::string paire4);
    static ChromoCell *additionCellule(ChromoCell &c1, ChromoCell &c2);
    ChromoCell* operator+(const Cell& autre) override;
    void afficher(std::ostream &os) const override;
};

#endif // ARBORESCENCE_GENETIQUE_CHROMOCELL_H
