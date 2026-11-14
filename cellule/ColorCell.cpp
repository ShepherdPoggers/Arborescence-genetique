#include "ColorCell.h"
#include <stdexcept>

using namespace std;

ColorCell::ColorCell(string unNom, int unRouge,
                     int unVert, int unBleu) : Cell(unNom),
                                               rouge(unRouge), vert(unVert), bleu(unBleu)
{
}

ColorCell *ColorCell::operator+(const Cell &autre)
{
    const ColorCell *color = dynamic_cast<const ColorCell *>(&autre);
    if (color == nullptr)
        throw invalid_argument("Une ColorCell doit être additionner à une ColorCell.");

    int nouveauRouge;
    int nouveauVert;
    int nouveauBleu;
    ColorCell *c3 = nullptr;

    nouveauRouge = (rouge + color->rouge) / 2;
    nouveauVert = (vert + color->vert) / 2;
    nouveauBleu = (bleu + color->bleu) / 2;

    c3 = new ColorCell("NouvColorCell", nouveauRouge,
                       nouveauVert, nouveauBleu);
    return c3;
}

void ColorCell::afficher()
{
    cout << this->rouge << " " << this->vert << " " << this->bleu << endl;
}

void ColorCell::afficher(std::ostream& os) const
{
    Cell::afficher(os);
    os << " : " << rouge << " " << vert << " " << bleu;
}
