#ifndef ARBORESCENCE_GENETIQUE_COLORCELL_H
#define ARBORESCENCE_GENETIQUE_COLORCELL_H

#include <ostream>
#include <string>

class ColorCell
{
private:
    std::string nom;
    int rouge;
    int vert;
    int bleu;

public:
    ColorCell(std::string unNom, int rouge, int vert, int bleu);
    int getRouge() const { return rouge; }
    int getVert() const { return vert; }
    int getBleu() const { return bleu; }
    ColorCell* operator+(const ColorCell& autre) const;
    void afficher() const;
    void afficher(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const ColorCell& cellule);
};

#endif
