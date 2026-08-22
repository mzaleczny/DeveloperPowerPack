#include "Tilc/Utils/MemoryView.h"


Tilc::TMemoryView::TMemoryView(void* pointer, size_t size, size_t offset)
{
    SetMemory(pointer, size);
    m_Offset = offset;
}

Tilc::TMemoryView::~TMemoryView()
{
}

void Tilc::TMemoryView::SetMemory(void* pointer, size_t size)
{
    m_Pointer = pointer;
    m_Size = size;
}

Tilc::TExtString Tilc::TMemoryView::GetStringOfBytes(bool WithOffsets, bool WithASCII)
{
    Tilc::TExtString result;
    Tilc::TExtString line;
    Tilc::TExtString ascii;
    Tilc::TExtString indentation = "  ";
    Tilc::TExtString spacing = "    ";
    Tilc::TExtString asciiIndentation = indentation.substr(0, indentation.length()/2);
    
    size_t count = m_Size;
    unsigned char* ptr = static_cast<unsigned char*>(m_Pointer);
    unsigned char ch;
    size_t currentOffset = m_Offset;
    line = "";
    for (size_t i = 0; i < count; ++i)
    {
        line += Tilc::FormatHexValue(ptr[i], Tilc::FORMAT_ALIGN_LEFT, '0', 2, false, false, true, 2);
        if (WithASCII)
        {
            ch = ptr[i];
            if (ch < 32 || ch > 127)
            {
                ch = '.';
            }
            ascii.push_back(ch);
        }
        if ((i+1)%16 > 0)
        {
            if ((i+1)%8 == 0)
            {
                line += indentation + indentation;
                if (WithASCII)
                {
                    ascii += asciiIndentation;
                }
            }
            else
            {
                line += indentation;
            }
        }
        else
        {
            if (WithASCII)
            {
                line += spacing + ascii;
                ascii = "";
            }
            if (WithOffsets)
            {
                line = FormatHexValue(static_cast<int>(currentOffset), Tilc::FORMAT_ALIGN_LEFT, '0', 8, false, false, true, 8) + ":  " + line;
                currentOffset += 16;
            }
            result += line + "\n";
            line = "";
        }
    }
    
    if (!line.empty())
    {
        if (WithASCII)
        {
            // we must pad line with spaces
            size_t padCount = 16 - m_Size % 16;
            for (size_t i = 0; i < padCount-1; i++)
            {
                line += "  " + indentation;
            }

            if (16-padCount < 8)
            {
                line += indentation + "  ";
            }
            else if (padCount == 8)
            {
                line += indentation;
            }
            else
            {
                line += "  ";
            }
            line += spacing + ascii;
            ascii = "";
        }
        if (WithOffsets)
        {
            line = FormatHexValue(static_cast<int>(currentOffset), Tilc::FORMAT_ALIGN_LEFT, '0', 8, false, false, true, 8) + ":  " + line;
            currentOffset += 16;
        }
        result += line;
    }
    
    result.RTrim();
    return result;
}

Tilc::TExtString Tilc::TMemoryView::GetStringOfWords(bool WithOffsets, bool WithASCII)
{
    Tilc::TExtString result;
    Tilc::TExtString ascii;
    Tilc::TExtString indentation = "  ";
    Tilc::TExtString spacing = "    ";
    Tilc::TExtString asciiIndentation = indentation.substr(0, indentation.length()/2);
    
    size_t count = m_Size / sizeof(uint16_t);
    uint16_t* ptr = (uint16_t*)m_Pointer;
    uint8_t* ptrb;
    unsigned char ch;
    for (size_t i = 0; i < count; ++i)
    {
        result += FormatHexValue(ptr[i], Tilc::FORMAT_ALIGN_LEFT, '0', 4, false, false, true, 4);
        if (WithASCII)
        {
            ptrb = reinterpret_cast<uint8_t*>(&ptr[i]);
            ch = ptrb[i];
            if (ch == 0 || ch > 127)
            {
                ch = '.';
            }
            ascii.push_back(ch);
            
            ch = ptrb[i+1];
            if (ch == 0 || ch > 127)
            {
                ch = '.';
            }
            ascii.push_back(ch);
        }
        if ((i+1)%8 > 0)
        {
            if ((i+1)%4 == 0)
            {
                result += indentation + indentation;
                if (WithASCII)
                {
                    ascii += asciiIndentation;
                }
            }
            else
            {
                result += indentation;
            }
        }
        else
        {
            if (WithASCII)
            {
                result += spacing + ascii;
                ascii = "";
            }
            result += "\n";
        }
    }
    
    /* TODO as in this->getStringOfBytes
    if (line != COMMON_EMPTY_STRING) {
        if (withASCII) {
            // we must pad line with spaces
            size_t padCount = 8 - this->_size % 8;
            for (size_t i = 0; i < padCount-1; i++) {
                line += Tilc::TExtString(L"  ") + indentation;
            }
            if (8-padCount < 4) {
                line += indentation + Tilc::TExtString(L"  ");
            } else if (padCount == 4) {
                line += indentation;
            } else {
                line += Tilc::TExtString(L"  ");
            }
            line += spacing + ascii;
            ascii = COMMON_EMPTY_STRING;
        }
        if (withOffsets) {
            line = FormatHexValue(currentOffset, FORMAT_ALIGN_LEFT, L'0', 8, FALSE, FALSE, TRUE, 8) + L":  " + line;
            currentOffset += 16;
        }
        result += line;
    }
    */
    result.RTrim();
    return result;
}

Tilc::TExtString Tilc::TMemoryView::GetStringOfDwords(bool WithOffsets, bool WithASCII)
{
    Tilc::TExtString result;
    Tilc::TExtString ascii;
    Tilc::TExtString indentation = "  ";
    Tilc::TExtString spacing = "    ";
    Tilc::TExtString asciiIndentation = indentation.substr(0, indentation.length()/2);
    
    size_t count = m_Size / sizeof(uint32_t);
    uint32_t* ptr = (uint32_t*)m_Pointer;
    uint8_t* ptrb;
    unsigned char ch;
    for (size_t i = 0; i < count; ++i)
    {
        result += FormatHexValue(ptr[i], Tilc::FORMAT_ALIGN_LEFT, '0', 8, false, false, true, 8);
        if (WithASCII)
        {
            ptrb = reinterpret_cast<uint8_t*>(&ptr[i]);
            ch = ptrb[i];
            if (ch == 0 || ch > 127)
            {
                ch = '.';
            }
            ascii.push_back(ch);
            
            ch = ptrb[i+1];
            if (ch == 0 || ch > 127)
            {
                ch = '.';
            }
            ascii.push_back(ch);
            
            ch = ptrb[i+2];
            if (ch == 0 || ch > 127)
            {
                ch = '.';
            }
            ascii.push_back(ch);
            
            ch = ptrb[i+3];
            if (ch == 0 || ch > 127)
            {
                ch = '.';
            }
            ascii.push_back(ch);
        }
        if ((i+1)%4 > 0)
        {
            if ((i+1)%2 == 0)
            {
                result += indentation + indentation;
                if (WithASCII)
                {
                    ascii += asciiIndentation;
                }
            }
            else
            {
                result += indentation;
            }
        }
        else
        {
            if (WithASCII)
            {
                result += spacing + ascii;
                ascii = "";
            }
            result += "\n";
        }
    }
    
    /* TODO as in this->getStringOfBytes
    if (line != COMMON_EMPTY_STRING) {
        if (withASCII) {
            // we must pad line with spaces
            size_t padCount = 4 - this->_size % 4;
            for (size_t i = 0; i < padCount-1; i++) {
                line += Tilc::TExtString(L"  ") + indentation;
            }
            if (4-padCount < 2) {
                line += indentation + Tilc::TExtString(L"  ");
            } else if (padCount == 2) {
                line += indentation;
            } else {
                line += Tilc::TExtString(L"  ");
            }
            line += spacing + ascii;
            ascii = COMMON_EMPTY_STRING;
        }
        if (withOffsets) {
            line = FormatHexValue(currentOffset, FORMAT_ALIGN_LEFT, L'0', 8, FALSE, FALSE, TRUE, 8) + L":  " + line;
            currentOffset += 16;
        }
        result += line;
    }
    */
    result.RTrim();
    return result;
}

std::ostream& Tilc::TMemoryView::Print(std::ostream& out, Tilc::TExtString indent, bool dumpAsBytes, bool dumpAsWords, bool dumpAsDwords)
{
    Tilc::TExtString output = GetDump(indent, dumpAsBytes, dumpAsWords, dumpAsDwords);
    if (!output.empty())
    {
        out << output;
    }
    return out;
}

Tilc::TExtString Tilc::TMemoryView::GetDump(Tilc::TExtString indent, bool dumpAsBytes, bool dumpAsWords, bool dumpAsDwords)
{
    if (dumpAsBytes || dumpAsWords || dumpAsDwords)
    {
        Tilc::TExtString output;
        Tilc::TExtString dump;
        if (dumpAsBytes)
        {
            output += indent + "BYTE DUMP:\n";
            dump = GetStringOfBytes();
            dump.PrependEachLine((indent + "    ").c_str());
            output += dump + "\n";
        }
        if (dumpAsWords)
        {
            output += indent + "WORD DUMP:\n";
            dump = GetStringOfWords();
            dump.PrependEachLine((indent + "    ").c_str());
            output += dump + "\n";
        }
        if (dumpAsDwords)
        {
            output += indent + "size_t DUMP:\n";
            dump = GetStringOfDwords();
            dump.PrependEachLine((indent + "    ").c_str());
            output += dump + "\n";
        }    
        return output;
    }
    
    return "";
}

Tilc::TExtString Tilc::TMemoryView::ToString()
{
    return GetDump("", true, false, false);
}
