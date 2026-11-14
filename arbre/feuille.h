#pragma once
#include <ostream>
#include <type_traits>

template <class T>
class Arbre;

template <class T>
class Feuille
{
    friend class Arbre<T>;

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

    Feuille<T>* operator+(Feuille<T>& autre)
    {
        T temp = *valeur + *autre.valeur;
        return new Feuille<T>(temp, this, &autre);
    }

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
