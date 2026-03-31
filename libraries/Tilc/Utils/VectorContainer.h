#pragma once

#include "Tilc/Utils/ExtString.h"
#include <iostream>
#include <vector>
#include <algorithm>

namespace Tilc
{

#ifndef UINTMAX_MAX
#define UINTMAX_MAX __UINTMAX_MAX__
#endif

    template <typename T>
    class TExtVectorContainer : public std::vector<T>
    {
    public:
        using std::vector<T>::vector;
        TExtVectorContainer() : std::vector<T>(), m_KeepSorted(false) {};
        TExtVectorContainer(bool Sorted) : std::vector<T>(), m_KeepSorted(Sorted) {};
        virtual ~TExtVectorContainer() {}

        // Dodaje element do wektora. Jesli m_KeepSorted==true, to w odpowiednim miejscu a w przeciwnym razie na koncu.
        void push_back(const T& Elem);
        // Jesli m_KeepSorted==true, to zwracany jest indeks elementu znaleziony metoda BinarySearch.
        // Jesli takiego elementu nie ma, to jesli ReturnInsertionIndex == true, to zwracany jest Indeks, na ktorym nalezy dodac ten element zeby wektor nadal byl posortowany.
        // A wprzciwnym razie zwracana jest wartosc MAX oznaczajaca brak elementu w wektorze.
        size_t BinarySearch(const T& Elem, bool ReturnInsertionIndex = false) const;
        // Zwraca indeks elementu w wektorze lub wartosc MAX jesli go tam nie bylo.
        size_t IndexOf(const T& Elem) const;
        // zwraca true jesli Element jest w wektorze.
        bool Contains(const T& Elem) const { return IndexOf(Elem) < UINTMAX_MAX; }
        // usuwa pierwszy element w wektorze
        bool Remove(T First)
        {
            size_t Index = IndexOf(First);
            if (Index == UINTMAX_MAX) return false;
            std::vector<T>::erase(std::vector<T>::begin() + Index);
            return true;
        }
        // zwraca ostatni element
        T& Last()
        {
            return (*this)[this->size()-1];
        }
        // Usuwa wszystkie wystapienia danej wartosci. Zwraca ilosc usunietych elementow
        size_t RemoveAll(T Elem);
        // Zwraca string z wszystkimi wartosciami wektora rozdzielonymi przecinkami.
        Tilc::TExtString GetString() const;
        // Ustawia flage czy wektor ma byc posortowany czy nie. Jesli ustawiono ta flage, to dodatkow prewencyjnie sortuje wektor.
        void SetSorted(bool Sorted);
    private:
        // Czy wektor ma utrzymywac posortowana zawartosc elementow
        bool m_KeepSorted;
    };




    // ********************************************************************************************************
    // UWAGA!!! definicje metod powyzszej klasy musza byc w tym samym pliku, bo inaczej kod sie nie skompiluje.
    // ********************************************************************************************************
    template <typename T>
    std::ostream& operator<<(std::ostream& cout, const Tilc::TExtVectorContainer<T>& v)
    {
        // W przeciwnym razie musimy przejrzec potencjalnie caly wektor
        std::cout << "[";
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (i > 0) std::cout << " ";
            cout << v[i];
        }
        std::cout << "]" << std::endl;
        return cout;
    }

    template <typename T>
    void Tilc::TExtVectorContainer<T>::push_back(const T& Elem)
    {
        // Jesli nie utrzymujemy porzadku, to po prostu dokladamy element na koniec
        if (!m_KeepSorted)
        {
            std::vector<T>::push_back(Elem);
            return;
        }

        size_t Index = BinarySearch(Elem, true);
        if (Index == UINTMAX_MAX)
        {
            std::vector<T>::push_back(Elem);
        }
        else
        {
            std::vector<T>::insert(std::vector<T>::begin() + Index, Elem);
        }
    }

    template <typename T>
    size_t Tilc::TExtVectorContainer<T>::BinarySearch(const T& Elem, bool ReturnInsertionIndex) const
    {
        // Jesli wektor nie jest posortowany, to zwracamy wartosc oznaczajaca brak elementu
        if (!m_KeepSorted)
        {
            return UINTMAX_MAX;
        }

        if (std::vector<T>::size() == 0)
        {
            return 0;
        }

        size_t Size = std::vector<T>::size();
        size_t Left = 0;
        size_t Right = Size - 1;
        size_t Mid;
        while (Left <= Right)
        {
            Mid = (Left + Right) / 2;
            if (Mid < 0 || Mid >= Size) break;
            if ((*this)[Mid] < Elem)
            {
                Left = Mid + 1;
            }
            else if ((*this)[Mid] > Elem)
            {
                Right = Mid - 1;
            }
            else
            {
                // jesli chcemy zwrocic indeks na pozycji ktorej ma byc wstawiony nowy element, to idziemy za ostatni znaleziony element jesli jest ich kilka pod rzad takich samych
                if (ReturnInsertionIndex)
                {
                    while (Mid >= 0 && Mid < std::vector<T>::size() && (*this)[Mid] == Elem)
                    {
                        Mid++;
                    }
                }
                return Mid;
            }
        }

        if (ReturnInsertionIndex)
        {
            // jesli chcemy zwrocic indeks na pozycji ktorej ma byc wstawiony nowy element, to idziemy za ostatni znaleziony element jesli jest ich kilka pod rzad takich samych
            while (Left >= 0 && Left < std::vector<T>::size() && (*this)[Left] == Elem)
            {
                Left++;
            }
            return Left;
        }

        return UINTMAX_MAX;
    }

    template <typename T>
    size_t Tilc::TExtVectorContainer<T>::IndexOf(const T& Elem) const
    {
        // Jesli wektor jest posortowany, to znalezienie elementu jest szybkie
        if (m_KeepSorted)
        {
            return BinarySearch(Elem);
        }

        // W przeciwnym razie musimy przejrzec potencjalnie caly wektor
        for (size_t i = 0; i < std::vector<T>::size(); ++i)
        {
            if ((*this)[i] == Elem)
            {
                return true;
            }
        }
        return UINTMAX_MAX;
    }

    template <typename T>
    size_t Tilc::TExtVectorContainer<T>::RemoveAll(T Elem)
    {
        size_t Index = IndexOf(Elem);
        if (Index == UINTMAX_MAX) return 0;

        size_t Count = 0;
        auto it = std::vector<T>::begin() + Index;
        while (it != std::vector<T>::end())
        {
            if (*it == Elem)
            {
                std::vector<T>::erase(std::vector<T>::begin() + Index);
                ++Count;
                it = std::vector<T>::begin() + Index;
            }
        }
        return Count;
    }

    template <typename T>
    Tilc::TExtString Tilc::TExtVectorContainer<T>::GetString() const
    {
        Tilc::TExtString s;
        for (size_t i = 0; i < std::vector<T>::size(); ++i)
        {
            if (s != "")
            {
                s += ", ";
            }
            s += std::to_string((*this)[i]);
        }
        return s;
    }

    template <typename T>
    void Tilc::TExtVectorContainer<T>::SetSorted(bool Sorted)
    {
        m_KeepSorted = Sorted;
        if (!m_KeepSorted)
        {
            return;
        }
        // W przeciwnym razie musimy prewencyjnie posortowac wektor
        std::sort(std::vector<T>::begin(), std::vector<T>::end());
    }
}
