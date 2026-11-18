#include "ChromoCell.h"

#include <regex>
#include <stdexcept>
#include <utility>

ChromoCell::ChromoCell(std::string unNom, std::string unePaire1, std::string unePaire2,
                       std::string unePaire3, std::string unePaire4)
    : nom(std::move(unNom)), paire1(std::move(unePaire1)), paire2(std::move(unePaire2)),
      paire3(std::move(unePaire3)), paire4(std::move(unePaire4))
{
    const std::regex regle("^[A-L]{2}$");
    if (!std::regex_match(paire1, regle) || !std::regex_match(paire2, regle) ||
        !std::regex_match(paire3, regle) || !std::regex_match(paire4, regle))
    {
        throw std::invalid_argument("Les paires doivent contenir 2 lettres majuscules entre A et L.");
    }
}

ChromoCell* ChromoCell::operator+(const ChromoCell& autre) const
{
    return new ChromoCell("NouvChromoCell",
        std::string(1, paire1[0]) + autre.paire1[1],
        std::string(1, paire2[0]) + autre.paire2[1],
        std::string(1, paire3[0]) + autre.paire3[1],
        std::string(1, paire4[0]) + autre.paire4[1]);
}

ChromoCell* ChromoCell::additionCellule(ChromoCell& c1, ChromoCell& c2)
{
    return c1 + c2;
}

void ChromoCell::afficher(std::ostream& os) const
{
    os << nom << " : " << paire1 << ' ' << paire2 << ' ' << paire3 << ' ' << paire4;
}

std::ostream& operator<<(std::ostream& os, const ChromoCell& cellule)
{
    cellule.afficher(os);
    return os;
}
