#pragma once
#include <iostream>
#include <string>

using namespace std;
#ifndef ARBORESCENCE_GENETIQUE_CHROMOCELL_H
#define ARBORESCENCE_GENETIQUE_CHROMOCELL_H

class ChromoCell {
private:
    string paire1;
    string paire2;
    string paire3;
    string paire4;
public:
    ChromoCell();
    ChromoCell(string paire1, string paire2, string paire3, string paire4);
    static ChromoCell additionCellule(ChromoCell c1, ChromoCell c2);
    void afficher();
};

#endif //ARBORESCENCE_GENETIQUE_CHROMOCELL_H
