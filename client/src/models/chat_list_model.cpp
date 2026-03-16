#include "chat_list_model.h"

ChatListModel::ChatListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ChatListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return chats_.size();
}

QVariant ChatListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= chats_.size())
        return {};

    const auto &entry = chats_.at(index.row());
    switch (role)
    {
    case UidRole:
        return entry.user.uid;
    case NameRole:
        return entry.user.name;
    case LastMessageRole:
        return entry.user.last_message;
    case TimeRole:
        return entry.user.time;
    case AvatarSourceRole:
        return entry.user.avatar_source;
    case UnreadCountRole:
        return entry.user.unread_count;
    }
    return {};
}

QHash<int, QByteArray> ChatListModel::roleNames() const
{
    return {
        {UidRole, "uid"},
        {NameRole, "name"},
        {LastMessageRole, "lastMessage"},
        {TimeRole, "time"},
        {AvatarSourceRole, "avatarSource"},
        {UnreadCountRole, "unreadCount"}};
}

void ChatListModel::addChat(int uid, const QString &name, const QString &last_message,
                            const QString &time, const QString &avatar_source,
                            int unread_count)
{
    beginInsertRows(QModelIndex(), chats_.size(), chats_.size());
    UserItem user{uid, name, avatar_source, false, false, last_message, time, unread_count};
    auto *msg_model = new ChatMessageModel(this);
    chats_.append({user, msg_model});
    endInsertRows();
}

void ChatListModel::clearChats()
{
    beginResetModel();
    for (auto &entry : chats_)
        delete entry.message_model;
    chats_.clear();
    page_ = 0;
    has_more_ = true;
    endResetModel();
    emit hasMoreChanged();
}

bool ChatListModel::hasMore() const
{
    return has_more_;
}

bool ChatListModel::loading() const
{
    return loading_;
}

void ChatListModel::loadMore()
{
    if (loading_ || !has_more_)
        return;

    loading_ = true;
    emit loadingChanged();

    // TODO: 替换为实际的网络请求/数据库查询

    loading_ = false;
    emit loadingChanged();
}

QString ChatListModel::getNameByUid(int uid) const
{
    for (const auto &entry : chats_)
    {
        if (entry.user.uid == uid)
            return entry.user.name;
    }
    return {};
}

bool ChatListModel::hasChat(int uid) const
{
    for (const auto &entry : chats_)
    {
        if (entry.user.uid == uid)
            return true;
    }
    return false;
}

ChatMessageModel *ChatListModel::getMessageModel(int uid)
{
    for (auto &entry : chats_)
    {
        if (entry.user.uid == uid)
            return entry.message_model;
    }
    return nullptr;
}

void ChatListModel::incrementUnread(int uid)
{
    for (int i = 0; i < chats_.size(); ++i)
    {
        if (chats_[i].user.uid == uid)
        {
            chats_[i].user.unread_count++;
            auto idx = index(i);
            emit dataChanged(idx, idx, {UnreadCountRole});
            return;
        }
    }
}

void ChatListModel::clearUnread(int uid)
{
    for (int i = 0; i < chats_.size(); ++i)
    {
        if (chats_[i].user.uid == uid)
        {
            if (chats_[i].user.unread_count == 0)
                return;
            chats_[i].user.unread_count = 0;
            auto idx = index(i);
            emit dataChanged(idx, idx, {UnreadCountRole});
            return;
        }
    }
}

void ChatListModel::removeChat(int uid)
{
    for (int i = 0; i < chats_.size(); ++i)
    {
        if (chats_[i].user.uid == uid)
        {
            beginRemoveRows(QModelIndex(), i, i);
            delete chats_[i].message_model;
            chats_.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}
