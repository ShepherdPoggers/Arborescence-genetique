
#pragma once
#include <iostream>
#include <string>

#ifndef CELL_H
#define CELL_H

class Cell
{
private:
    /* data */
    std::string nom;

public:
    Cell(std::string unNom);
    //virtual Cell* additionnerCellule(const Cell& c2);
    virtual Cell* operator+(const Cell& autre) = 0;
    virtual void afficher(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const Cell& cell);

    virtual ~Cell() = default;
};


#endif
