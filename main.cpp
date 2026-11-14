#include <iostream>
#include "arbre/arbre.h"
#include "cellule/cell.h"
#include "cellule/ChromoCell.h"
#include "cellule/ColorCell.h"
using namespace std;

int main()
{
    // TEST
    Cell* c1 = new ChromoCell("1", "AB", "CD", "EF", "GH");
    Cell* c2 = new ChromoCell("2", "IJ", "KL", "AB", "CD");
    Cell* c3 = new ChromoCell("3", "AJ", "CL", "EB", "GD");
    Cell* c4 = new ChromoCell("4", "KL", "IJ", "GH", "EF");
    Cell* c5 = new ChromoCell("5", "AL", "CJ", "EH", "GF");
    Cell* c6 = new ChromoCell("6", "IL", "KJ", "AH", "CF");

    Feuille<Cell*>* f1 = new Feuille<Cell*>(c1);
    Feuille<Cell*>* f2 = new Feuille<Cell*>(c2);
    Feuille<Cell*>* f3 = new Feuille<Cell*>(c3, f1, f2);
    Feuille<Cell*>* f4 = new Feuille<Cell*>(c4);
    Feuille<Cell*>* f5 = new Feuille<Cell*>(c5, f3, f4);
    Feuille<Cell*>* f6 = new Feuille<Cell*>(c6, f2, f5);
    Feuille<Cell*>* f7 = *f6 + *f4;

    Arbre<Cell*>* a1 = new Arbre<Cell*>(f7);
    a1->afficherArborescence();
    a1->reset();
    cout << a1->trouverAncetre(f1) << endl;
    cout << *a1 << endl;

    // TEST ColorCell
    Cell* couleur1 = new ColorCell("Rouge", 255, 0, 0);
    Cell* couleur2 = new ColorCell("Bleu", 0, 0, 255);
    Cell* couleur3 = *couleur1 + *couleur2;

    Feuille<Cell*>* feuilleCouleur1 = new Feuille<Cell*>(couleur1);
    Feuille<Cell*>* feuilleCouleur2 = new Feuille<Cell*>(couleur2);
    Feuille<Cell*>* feuilleCouleur3 = new Feuille<Cell*>(couleur3, feuilleCouleur1, feuilleCouleur2);

    Arbre<Cell*>* arbreCouleurs = new Arbre<Cell*>(feuilleCouleur3);
    cout << "\nArbre des ColorCell :" << endl;
    arbreCouleurs->afficherArborescence();
    arbreCouleurs->reset();
    cout << "Distance au parent Rouge : "
         << arbreCouleurs->trouverAncetre(feuilleCouleur1) << endl;

    delete arbreCouleurs;
    delete feuilleCouleur3;
    delete feuilleCouleur2;
    delete feuilleCouleur1;
    delete couleur3;
    delete couleur2;
    delete couleur1;

    delete a1;
    delete f6;
    delete f5;
    delete f4;
    delete f3;
    delete f2;
    delete f1;
    delete c6;
    delete c5;
    delete c4;
    delete c3;
    delete c2;
    delete c1;
    return 0;
}
