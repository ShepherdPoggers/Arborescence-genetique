#pragma once
#include <ostream>
#include <type_traits>

template <class T>
class Arbre;

template <class T>
class Population;

template <class T>
class Feuille
{
    friend class Arbre<T>;
    template <class U>
    friend class Population;

private:
    T valeur;
    Feuille<T>* parent1 = nullptr;
    Feuille<T>* parent2 = nullptr;

public:
    Feuille(const T& uneValeur,
            Feuille<T>* premierParent = nullptr,
            Feuille<T>* secondParent = nullptr): valeur(uneValeur),
          parent1(premierParent), parent2(secondParent)
    {
    }

    const Feuille<T>* getParent1() const { return parent1; }
    const Feuille<T>* getParent2() const { return parent2; }

    friend std::ostream& operator<<(std::ostream& os, const Feuille& feuille)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            if (feuille.valeur == nullptr)
                return os << "(vide)";

            return os << *feuille.valeur;
        }
        else
        {
            return os << feuille.valeur;
        }
    }
};
