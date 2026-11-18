#include "ColorCell.h"

#include <iostream>
#include <stdexcept>
#include <utility>

ColorCell::ColorCell(std::string unNom, int unRouge, int unVert, int unBleu)
    : nom(std::move(unNom)), rouge(unRouge), vert(unVert), bleu(unBleu)
{
    if (rouge < 0 || rouge > 255 || vert < 0 || vert > 255 || bleu < 0 || bleu > 255)
        throw std::invalid_argument("Les composantes RGB doivent etre entre 0 et 255.");
}

ColorCell* ColorCell::operator+(const ColorCell& autre) const
{
    return new ColorCell("NouvColorCell", (rouge + autre.rouge) / 2,
                         (vert + autre.vert) / 2, (bleu + autre.bleu) / 2);
}

void ColorCell::afficher() const
{
    std::cout << rouge << ' ' << vert << ' ' << bleu << '\n';
}

void ColorCell::afficher(std::ostream& os) const
{
    os << nom << " : " << rouge << ' ' << vert << ' ' << bleu;
}

std::ostream& operator<<(std::ostream& os, const ColorCell& cellule)
{
    cellule.afficher(os);
    return os;
}
