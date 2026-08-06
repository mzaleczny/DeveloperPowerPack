#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Apps
    {
        namespace Www
        {
            class TRequestHandler;

            class DECLSPEC TPayload
            {
            public:
                TPayload(TRequestHandler& RequestHandler);
                ~TPayload() = default;

            protected:
                TRequestHandler& rh;
                int m_Crc32;
                int m_Size;
                Tilc::TExtString m_Data;
            };
        }
    }
}
