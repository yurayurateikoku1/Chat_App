#include "chat_message_model.h"

ChatMessageModel::ChatMessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ChatMessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return messages_.size();
}

QVariant ChatMessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= messages_.size())
        return {};

    const auto &msg = messages_.at(index.row());
    switch (role)
    {
    case MessageRole:
        return msg.message;
    case TimeRole:
        return msg.time;
    case IsSelfRole:
        return msg.is_self;
    }
    return {};
}

QHash<int, QByteArray> ChatMessageModel::roleNames() const
{
    return {
        {MessageRole, "message"},
        {TimeRole, "time"},
        {IsSelfRole, "isSelf"}};
}

void ChatMessageModel::addMessage(const QString &msgid, const QString &message, const QString &time, bool is_self)
{
    beginInsertRows(QModelIndex(), messages_.size(), messages_.size());
    messages_.append({msgid, message, time, is_self});
    endInsertRows();
}

void ChatMessageModel::clearMessages()
{
    beginResetModel();
    messages_.clear();
    endResetModel();
}
