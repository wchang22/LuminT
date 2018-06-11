#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <QByteArray>

class Message
{
public:

    /*!
     * \brief The MessageID enum, specifies message types
     */
    enum class MessageID : uint8_t
    {
        INVALID,
        INFO,
        REQUEST,
        ACKNOWLEDGE,
        TEXT,
        ENUM_END
    };

    virtual ~Message() {}

    /*!
     * \brief type, pure virtual method to return message type
     * \return message type
     */
    virtual MessageID type() const = 0;

    /*!
     * \brief serialize, pure virtual method allows all messages to be serialized
     * \return QVector of bytes representing serialized message
     */
    virtual QByteArray serialize() = 0;
};

#endif // MESSAGE_HPP
