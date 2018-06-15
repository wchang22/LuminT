#include "utilities.hpp"

Utilities::Utilities(QClipboard *clipboard)
    : clipboard(nullptr)
{
    this->clipboard = clipboard;
}

QString Utilities::paste() const
{
    return clipboard->text();
}

void Utilities::copy(QString text)
{
    clipboard->setText(text);
}
