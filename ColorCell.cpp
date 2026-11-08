#include "ColorCell.h"

using namespace std;

ColorCell::ColorCell(int rouge, int vert, int bleu) {
    this->rouge = rouge;
    this->vert = vert;
    this->bleu = bleu;
}

ColorCell::ColorCell() {
    this->rouge = 0;
    this->vert = 0;
    this->bleu = 0;
}

ColorCell ColorCell:: additionCellule(ColorCell c1, ColorCell c2) {
    int nouveauRouge;
    int nouveauVert;
    int nouveauBleu;
    ColorCell c3;

    nouveauRouge = (c1.rouge + c2.rouge)/2;
    nouveauVert = (c1.vert + c2.vert)/2;
    nouveauBleu = (c1.bleu + c2.bleu)/2;

    c3 = ColorCell(nouveauRouge, nouveauVert, nouveauBleu);

    return c3;
}

void ColorCell::afficher() {
    cout<< this->rouge << " " << this->vert << " " << this->bleu << endl;
}