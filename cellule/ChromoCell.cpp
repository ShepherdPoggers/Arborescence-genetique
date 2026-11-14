#include "ChromoCell.h"
#include <regex>
#include <stdexcept>
using namespace std;

ChromoCell::ChromoCell(string unNom, string paire1, string paire2, 
    string paire3, string paire4) : Cell(unNom)
{
    // 2 lettres maj de A jusqu'a L
    regex regle("^[A-L]{2}$");
    bool regleRegex = regex_match(paire1, regle) && regex_match(paire2, regle) && regex_match(paire3, regle) && regex_match(paire4, regle);

    if (regleRegex)
    {
        this->paire1 = paire1;
        this->paire2 = paire2;
        this->paire3 = paire3;
        this->paire4 = paire4;
    }
    else
    {
        throw invalid_argument("Les paires doivent contenir 2 lettres majuscules entre A et L.");
    }
}



void ChromoCell::afficher(ostream &os) const
{
    Cell::afficher(os); // Affiche le nom
    os << " :" << " " << paire1
       << " " << paire2
       << " " << paire3
       << " " << paire4;
}

ostream &operator<<(ostream &os, const ChromoCell &cell)
{
    cell.afficher(os);
    return os;
}

ChromoCell* ChromoCell::operator+(const Cell &autre)
{
    const ChromoCell *chromo = dynamic_cast<const ChromoCell *>(&autre);

    if (chromo == nullptr)
    {
        throw 
        invalid_argument("Une ChromoCell doit être additionner à une ChromoCell.");
    }

    string nouvellePaire1;
    string nouvellePaire2;
    string nouvellePaire3;
    string nouvellePaire4;
    ChromoCell *c3 = nullptr;

    nouvellePaire1 = string(1, this->paire1[0]) + chromo->paire1[1];
    nouvellePaire2 = string(1, this->paire2[0]) + chromo->paire2[1];
    nouvellePaire3 = string(1, this->paire3[0]) + chromo->paire3[1];
    nouvellePaire4 = string(1, this->paire4[0]) + chromo->paire4[1];

    // Il faut changer la manière que le nom est attribué
    c3 = new ChromoCell("bla", nouvellePaire1, nouvellePaire2, nouvellePaire3, nouvellePaire4);

    return c3;

}
