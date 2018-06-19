#include "utilities.hpp"

Utilities::Utilities(QClipboard *clipboard)
    : clipboard(clipboard)
{
}

QByteArray Utilities::uint32ToByteArray(uint32_t integer)
{
    QByteArray byteArray;
    for (int i = 0; i < 4; i++)
        byteArray.prepend(static_cast<char>((integer >> i * 8) & 0xFF));

    return byteArray;
}

uint32_t Utilities::byteArrayToUint32(QByteArray byteArray)
{
    uint32_t integer = 0;
    for (int i = 0; i < 4; i++)
        integer += byteArray.at(i) << ((3 - i) * 8);

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
