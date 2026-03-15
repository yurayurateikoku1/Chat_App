#pragma once
#include <QObject>
#include <QQmlEngine>
#include "../common.h"
#include "../models/chat_message_model.h"

class ChatPage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(ChatMessageModel *chatMessageModel READ chatMessageModel CONSTANT)

public:
    explicit ChatPage(QObject *parent = nullptr);

    ChatMessageModel *chatMessageModel() const;

private:
    ChatMessageModel *chat_message_model_;
};
