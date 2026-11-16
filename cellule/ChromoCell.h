#ifndef ARBORESCENCE_GENETIQUE_CHROMOCELL_H
#define ARBORESCENCE_GENETIQUE_CHROMOCELL_H

#include <ostream>
#include <string>

class ChromoCell
{
private:
    std::string nom;
    std::string paire1;
    std::string paire2;
    std::string paire3;
    std::string paire4;

public:
    ChromoCell(std::string unNom, std::string paire1, std::string paire2,
               std::string paire3, std::string paire4);
    const std::string& getPaire1() const { return paire1; }
    const std::string& getPaire2() const { return paire2; }
    const std::string& getPaire3() const { return paire3; }
    const std::string& getPaire4() const { return paire4; }
    static ChromoCell* additionCellule(ChromoCell& c1, ChromoCell& c2);
    ChromoCell* operator+(const ChromoCell& autre) const;
    void afficher(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const ChromoCell& cellule);
};

#endif
