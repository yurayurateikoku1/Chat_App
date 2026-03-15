#include "chat_page.h"

ChatPage::ChatPage(QObject *parent)
    : QObject(parent), chat_message_model_(new ChatMessageModel(this))
{
}

ChatMessageModel *ChatPage::chatMessageModel() const
{
    return chat_message_model_;
}
