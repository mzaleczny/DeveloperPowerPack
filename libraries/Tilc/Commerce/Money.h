#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Commerce
    {

        class DECLSPEC TMoney
        {
        public:
            TMoney() {};
            TMoney(int Amount, int Decimals = 0, char Separator = ',') : m_Amount(Amount), m_Decimals(Decimals), m_Separator(Separator) {};
            ~TMoney() {}

            inline int GetAmount() const {
                return m_Amount;
            }

            inline int GetDecimals() const {
                return m_Decimals;
            }

            char GetSeparator() const {
                return m_Separator;
            }

            inline int GetTotalAmountInt() const {
                return m_Amount * 100 + +m_Decimals;
            }

            inline double GetTotalAmount() const {
                return (double)m_Amount + (double)m_Decimals / 100;
            }

            inline void SetTotal(int Total) {
                m_Amount = Total / 100;
                m_Decimals = Total % 100;
            }

            inline void SetTotal(double Total) {
                m_Amount = floor(Total);
                m_Decimals = floor((Total-m_Amount) * 100);
            }

            inline void SetAmount(int Amount) {
                m_Amount = Amount;
            }

            inline void SetDecimals(int Decimals)
            {
                m_Decimals = Decimals;
            }

            TMoney& operator=(double Total);
            TMoney& operator=(Tilc::TExtString str);
            TMoney operator+(Tilc::TExtString str);
            TMoney operator-(Tilc::TExtString str);
            TMoney operator/(int i);
            // zwraca procent z aktualnej kwoty w formie łańcucha i równoczeťnie zmienia zawartość obiektu
            Tilc::TExtString Procent(double i);

            Tilc::TExtString ToString() const;
            Tilc::TExtString ToSpeech() const;
            Tilc::TExtString GetFormattedKasa(Tilc::TExtString format = "") const;

        private:
            //ilość
            int m_Amount{};
            //ilość po przecinku
            int m_Decimals{};
            //symbol reprezentujący kropkę dziesiętną
            char m_Separator{ ',' };
            char AmountCurrency[8]{ "zł" };
            char DecimalsCurrency[8]{ "gr" };
        };

    }
}

inline bool operator==(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
    return m1.GetAmount() == m2.GetAmount() && m1.GetDecimals() == m2.GetDecimals();
}
inline bool operator<(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
    return m1.GetAmount() * 100 + m1.GetDecimals() < m2.GetAmount() * 100 + m2.GetDecimals();
}
inline bool operator>(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
    return m1.GetAmount() * 100 + m1.GetDecimals() > m2.GetAmount() * 100 + m2.GetDecimals();
}
inline Tilc::Commerce::TMoney operator+(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
    return m1 + m2;
}
inline Tilc::Commerce::TMoney operator-(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
    return m1 - m2;
}
