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
        return entry.user->uid;
    case NameRole:
        return entry.user->name;
    case IconRole:
        return entry.user->icon;
    case LastMsgRole:
        return entry.user->last_msg;
    case TimeRole:
        return entry.user->time;
    case UnreadCntRole:
        return entry.user->unread_cnt;
    }
    return {};
}

QHash<int, QByteArray> ChatListModel::roleNames() const
{
    return {
        {UidRole, "uid"},
        {NameRole, "name"},
        {IconRole, "icon"},
        {LastMsgRole, "lastMsg"},
        {TimeRole, "time"},
        {UnreadCntRole, "unreadCnt"}};
}

void ChatListModel::addChat(std::shared_ptr<FriendInfo> friend_info)
{
    beginInsertRows(QModelIndex(), chats_.size(), chats_.size());
    auto *msg_model = new ChatMessageModel(this);
    chats_.append({friend_info, msg_model});
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
        if (entry.user->uid == uid)
            return entry.user->name;
    }
    return {};
}

bool ChatListModel::hasChat(int uid) const
{
    for (const auto &entry : chats_)
    {
        if (entry.user->uid == uid)
            return true;
    }
    return false;
}

ChatMessageModel *ChatListModel::getMessageModel(int uid)
{
    for (auto &entry : chats_)
    {
        if (entry.user->uid == uid)
            return entry.message_model;
    }
    return nullptr;
}

void ChatListModel::incrementUnread(int uid)
{
    if (uid == current_chat_uid_)
        return;

    for (int i = 0; i < chats_.size(); ++i)
    {
        if (chats_[i].user->uid == uid)
        {
            chats_[i].user->unread_cnt++;
            auto idx = index(i);
            emit dataChanged(idx, idx, {UnreadCntRole});
            return;
        }
    }
}

void ChatListModel::setCurrentChatUid(int uid)
{
    current_chat_uid_ = uid;
}

int ChatListModel::currentChatUid() const
{
    return current_chat_uid_;
}

void ChatListModel::clearUnread(int uid)
{
    for (int i = 0; i < chats_.size(); ++i)
    {
        if (chats_[i].user->uid == uid)
        {
            if (chats_[i].user->unread_cnt == 0)
                return;
            chats_[i].user->unread_cnt = 0;
            auto idx = index(i);
            emit dataChanged(idx, idx, {UnreadCntRole});
            return;
        }
    }
}

void ChatListModel::removeChat(int uid)
{
    for (int i = 0; i < chats_.size(); ++i)
    {
        if (chats_[i].user->uid == uid)
        {
            beginRemoveRows(QModelIndex(), i, i);
            delete chats_[i].message_model;
            chats_.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}
