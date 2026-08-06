#include <cmath>
#include "Tilc/Utils/Archives.h"
#include "Tilc/Apps/Www/RequestHandler.h"
#include "Tilc/Apps/Www/Payload.h"
#include "Tilc/Utils/Base64.h"
#include "Tilc/Utils/FileUtils.h"

Tilc::Apps::Www::TPayload::TPayload(Tilc::Apps::Www::TRequestHandler& RequestHandler)
    : rh(RequestHandler)
{
    m_Crc32 = 0;
    m_Size = 0;

    Tilc::TExtString Data;
    Tilc::TBase64::Decode(rh.Body, Data);

    int BearerLen = static_cast<int>(rh.Bearer.length());
    for (int i = 0; i < Data.size() && i < BearerLen; ++i)
    {
        Data[i] = Data[i] ^ rh.Bearer[i % BearerLen];
    }
/*
    for (int k = 0; rh.GetRequest()->envp[k] != nullptr; k+=2)
    {
        if (rh.GetRequest()->envp[k+1])
        {
            rh << rh.GetRequest()->envp[k] << ": " << rh.GetRequest()->envp[k+1] << "<br/>\n";
        }
    }
*/
    if (Data.size() >= 8)
    {
        Tilc::TExtString s;
        int i;

        for (i = 0; i < 4; ++i)
        {
            m_Crc32 += static_cast<unsigned char>(Data[i]) * pow(256, i);
        }

        for (i = 4; i < 8; ++i)
        {
            m_Size += static_cast<unsigned char>(Data[i]) * pow(256, i-4);
        }
    }
    Data.assign(Data.data() + 8, Data.size() - 8);
    Tilc::TExtString Chars = "l[XO%q1\\\\*6P<QUI=waj#O\\]_YkA%rx</({o77e6O9rJwPk$STEmzG(3IY|,dm-AR|70CFC\%3jaf)N325Nb5MR}dIlyw[SSz,ICst<@F)Mp.F\'D|J^7/Q9d&F\"Nf21[9Ib7x$24xq,!r9A.%|RnI&C*03l3*V)v[PG5:&FS%J\\y])v}apa\%92Cm:\\,*phDzL.1#Z?z[4hPnH3?a%1{mgW!$Gl2y.\'-571h\%:od*;;au%Vn/$rPfK?1MEXW^>HbF";
    for (int i = 0; i < Data.size(); ++i)
    {
        Data[i] = Data[i] ^ Chars[i % Chars.length()];
    }
    int Result = Tilc::TArchive::GzUncompress(Data, rh.Body);
    // if sizes not match then nuke incoming data
    if (rh.Body.length() != m_Size)
    {
        rh.Body = "";
        rh << "ERROR:" + std::to_string(Result);
    }
}
