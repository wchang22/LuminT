#include "utilities.hpp"

Utilities::Utilities(QClipboard *clipboard)
    : clipboard(clipboard)
{
}

QByteArray Utilities::uint32ToByteArray(uint32_t integer)
{
    QByteArray byteArray;
    for (int i = 0; i < LuminT::UINT32_BYTE_REP; i++)
        byteArray.prepend(static_cast<char>(((integer >> (i * LuminT::BYTE))) & 0xFF));

    return byteArray;
}

uint32_t Utilities::byteArrayToUint32(QByteArray byteArray)
{
    uint32_t integer = 0;
    for (int i = 0; i < LuminT::UINT32_BYTE_REP; i++)
        integer += static_cast<uint8_t>(byteArray.at(i)) <<
                   ((LuminT::UINT32_BYTE_REP - i - 1) * LuminT::BYTE);

    return integer;
}

QString Utilities::paste() const
{
    return clipboard->text();
}

void Utilities::copy(QString text)
{
    clipboard->setText(text);
}
