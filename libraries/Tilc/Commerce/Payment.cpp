#include "Tilc/Commerce/Payment.h"


Tilc::Commerce::TPayment::TPayment()
{
    m_Config.resize(2);
}

Tilc::Commerce::TPayment::~TPayment()
{
}

void Tilc::Commerce::TPayment::AddConfig(EPaymentConfigType ConfigType, TPaymentConfig Config)
{
    m_Config[ConfigType] = Config;
}

