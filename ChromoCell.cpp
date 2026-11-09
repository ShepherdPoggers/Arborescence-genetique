#include "ChromoCell.h"
#include <regex>
#include <stdexcept>
using namespace std;

ChromoCell::ChromoCell() {
    this->paire1 = "AA";
    this->paire2 = "AA";
    this->paire3 = "AA";
    this->paire4 = "AA";
}

ChromoCell::ChromoCell(string paire1, string paire2, string paire3, string paire4) {
    //2 lettres maj de A jusqu'a L
    regex regle("^[A-L]{2}$");
    bool regleRegex = regex_match(paire1, regle) && regex_match(paire2, regle) && regex_match(paire3, regle) && regex_match(paire4, regle);

    if (regleRegex) {
        this->paire1 = paire1;
        this->paire2 = paire2;
        this->paire3 = paire3;
        this->paire4 = paire4;
    }
    else {
        throw invalid_argument("Les paires doivent contenir 2 lettres majuscules entre A et L.");
    }
}

ChromoCell ChromoCell::additionCellule(ChromoCell c1, ChromoCell c2) {
    string nouvellePaire1;
    string nouvellePaire2;
    string nouvellePaire3;
    string nouvellePaire4;
    ChromoCell c3;

    nouvellePaire1 = string(1, c1.paire1[0]) + c2.paire1[1];
    nouvellePaire2 = string(1, c1.paire2[0]) + c2.paire2[1];
    nouvellePaire3 = string(1, c1.paire3[0]) + c2.paire3[1];
    nouvellePaire4 = string(1, c1.paire4[0]) + c2.paire4[1];

    c3 = ChromoCell(nouvellePaire1, nouvellePaire2, nouvellePaire3, nouvellePaire4);

    return c3;
}

void ChromoCell::afficher() {
    cout << this->paire1 << " " << this->paire2 << " " << this->paire3 << " " << this->paire4 << endl;
}
