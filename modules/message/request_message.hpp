#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include "message.hpp"

/*!
 * \brief The RequestMessage class, request for info from recipient
 */

class RequestMessage : public Message
{
public:

    enum class Request : char
    {
        DEVICE_KEY,  // Receiver requests device key from Sender
        FILE_PACKET, // Receiver requests file packet from Sender
        CANCEL_FILE_TRANSFER,
        PAUSE_FILE_TRANSFER,
        ENUM_END,    // Marks end of enum, used in testing
    };

    RequestMessage(Request request);
    RequestMessage(Request request, const QByteArray &requestInfo);
    RequestMessage(QByteArray &requestBytes);

    ~RequestMessage();

    Message::MessageID type() const;

    QByteArray serialize();

    Request request;
    QByteArray requestInfo;
};

#endif // REQUEST_MESSAGE_HPP
