#include "cell.h"
using namespace std;

Cell::Cell(string unNom) : nom(unNom) {
                                        };




void Cell::afficher(std::ostream &os) const
{
    os << nom;
}

std::ostream &operator<<(std::ostream &os, const Cell &cell)
{
    cell.afficher(os);
    return os;
}
