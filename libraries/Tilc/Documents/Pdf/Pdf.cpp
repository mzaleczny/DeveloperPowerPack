#include "Tilc/Documents/Pdf/Pdf.h"
#include "Tilc/Utils/FileUtils.h"
#include <iostream>

Tilc::Documents::Pdf::TPDF::TPDF()
{
    std::cout << "TPDF()" << std::endl;
}

Tilc::Documents::Pdf::TPDF::~TPDF()
{
    std::cout << "~TPDF()" << std::endl;
}
