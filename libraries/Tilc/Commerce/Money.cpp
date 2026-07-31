#include "Tilc/Commerce/Money.h"
#include "Tilc/Utils/Log.h"
#include <fstream>

Tilc::Commerce::TMoney& Tilc::Commerce::TMoney::operator=(Tilc::Commerce::TMoney Total)
{
    m_Amount = Total.m_Amount;
    m_Decimals = Total.m_Decimals;
    m_Separator = Total.m_Separator;
    return *this;
}

Tilc::Commerce::TMoney& Tilc::Commerce::TMoney::operator=(double Total)
{
    if (Total < 0)
    {
        SetTotal(Total);
        return *this;
    }//if

    int a, d;
    // np. 12.78
    a = std::floor(Total); // części całkowite -> 12
    Total = Total - a;   // pozostaw części ułamkowe -> 0.78
    Total = Total * 100; // zrób z nich części całkowite -> 78
    d = std::round(Total); // zwróć je jako grosze
    if (d > 99)
    {
        a++;
        d = 0;
    }

    m_Amount = a;
    m_Decimals = d;

    return *this;
}

Tilc::Commerce::TMoney& Tilc::Commerce::TMoney::operator=(Tilc::TExtString str)
{
    Tilc::TExtString tmp;
    int i;

    // sprawdzaj każdy znak danego łańcucha
    for (i = 0; i < str.length(); ++i)
    {
        // Jeżeli jest spacją, to po prostu omiń
        if (str[i] == ' ')
        {
            continue;
        }
        // i jeśli jest cyfrą, do doklejaj do tmp
        else if (str[i] >= '0' && str[i] <= '9')
        {
            tmp += str[i];
        }
        // jeśli jest kropką, to wyjdź z pętli
        else if (str[i] == m_Separator)
        {
            break;
        }
        else
        {
            m_Amount = -1;
            m_Decimals = -1;
            return *this;
        }
    }
    // tmp zawiera teraz z│ote
    m_Amount = std::atoi(tmp.c_str());


    // ustaw Decimals
    if (i + 1 < str.length())
    {
        tmp.assign(&str[i + 1], 1);
        if (i + 2 < str.length())
        {
            tmp += str[i + 2];
        }
        else
        {
            tmp += "0";
        }
    }//if
    else
    {
        tmp = "0";
    }
    m_Decimals = std::atoi(tmp.c_str());
    return *this;
}

Tilc::Commerce::TMoney Tilc::Commerce::TMoney::operator+(Tilc::TExtString str)
{
    int a = m_Amount;
    int d = m_Decimals;

    Tilc::Commerce::TMoney m;
    m = str;
    if (m.m_Amount == -1 && m.m_Decimals == -1)
    {
        return *this;
    }

    m.m_Amount += a;
    m.m_Decimals += d;
    if (m_Decimals > 99)
    {
        ++m.m_Amount;
        m.m_Decimals -= 100;
    }
    return m;
}

Tilc::Commerce::TMoney Tilc::Commerce::TMoney::operator+(TMoney m)
{
    return TMoney(GetTotalAmountInt() + m.GetTotalAmountInt());
}

Tilc::Commerce::TMoney Tilc::Commerce::TMoney::operator-(Tilc::TExtString str)
{
    int a = m_Amount;
    int d = m_Decimals;

    Tilc::Commerce::TMoney m;
    m = str;
    if (m.m_Amount == -1 && m.m_Decimals == -1)
    {
        return *this;
    }

    int Total = GetTotalAmountInt() - m.GetTotalAmountInt();
    m.SetTotal(Total);
    return m;
}

Tilc::Commerce::TMoney Tilc::Commerce::TMoney::operator-(TMoney m)
{
    return TMoney(GetTotalAmountInt() - m.GetTotalAmountInt());
}

Tilc::Commerce::TMoney Tilc::Commerce::TMoney::operator*(int i)
{
    return Tilc::Commerce::TMoney(GetTotalAmountInt() * i);
}

Tilc::Commerce::TMoney Tilc::Commerce::TMoney::operator/(int i)
{
    double x;

    if (i == 0) return {};
    if (i == 1) return *this;

    // W przeciwnym razie wykonaj przeliczenia
    double Total = GetTotalAmount();
    int Amount = std::floor(Total);
    Total -= Amount;
    Total *= 100;
    int Decimals = Total;
    if (Decimals >= 100)
    {
        Decimals -= 100;
        ++Amount;
    }//if

    return Tilc::Commerce::TMoney(Amount, Decimals);
}

Tilc::TExtString Tilc::Commerce::TMoney::Procent(double i)
{
    double x;
    x = (GetTotalAmount() * i) / 100.0;
    SetTotal(x);
    return ToString();
}

Tilc::TExtString Tilc::Commerce::TMoney::ToString() const
{
    Tilc::TExtString tmp;

    tmp = std::to_string(m_Amount) + m_Separator;
    if (m_Decimals < 10)
    {
        tmp += "0";
    }
    tmp += std::to_string(m_Decimals);

    return tmp;
}

Tilc::TExtString Tilc::Commerce::TMoney::ToSpeech() const
{
    return Tilc::SlowDziewiec(m_Amount) + " " + AmountCurrency + "  " + std::to_string(m_Decimals) + "/100 " + DecimalsCurrency;
}

Tilc::TExtString Tilc::Commerce::TMoney::GetFormattedKasa(Tilc::TExtString format) const
{
    Tilc::TExtString res = "";
    Tilc::TExtString znak = " ";

    int i = 0;

    //Jeśli łańcuch zawierający format jest pusty, to ustaw domyślny format
    if (format.empty())
    {
        format = "%z zł %g gr";
    }

    //Przejdź przez cały łańcuch formatu i odpowiednio go interpretuj
    while (i < format.length())
    {
        if (format[i] == '%' && (i + 1 < format.length()))
        {
            ++i;
            switch (format[i])
            {
            case '%':
                res += "%"; //doklej ten znak do łańcucha wynikowego
                break;
            case 'z':
            case 'Z':
                res += std::to_string(m_Amount); //doklej kwotę do łańcucha wynikowego
                break;
            case 'g':
            case 'G':
                res += std::to_string(m_Decimals); //doklej grosze do łańcucha wynikowego
                break;
            }//switch
        }//if
        else
        {
            res += format[i];
        }//else

        ++i; //zwiększ licznik
    }//while

    return res;
}

