#include "user_list_model.h"

UserListModel::UserListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int UserListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return users_.size();
}

QVariant UserListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= users_.size())
        return {};

    const auto &user = users_.at(index.row());
    switch (role)
    {
    case UidRole:
        return user->uid;
    case NameRole:
        return user->name;
    case NickRole:
        return user->nick;
    case IconRole:
        return user->icon;
    case SexRole:
        return user->sex;
    case DescRole:
        return user->desc;
    case OnlineRole:
        return user->is_online;
    case IsFriendRole:
        return user->is_friend;
    case StatusRole:
        return user->status;
    }
    return {};
}

QHash<int, QByteArray> UserListModel::roleNames() const
{
    return {
        {UidRole, "uid"},
        {NameRole, "name"},
        {NickRole, "nick"},
        {IconRole, "icon"},
        {SexRole, "sex"},
        {DescRole, "desc"},
        {OnlineRole, "online"},
        {IsFriendRole, "isFriend"},
        {StatusRole, "status"}};
}

void UserListModel::addUser(std::shared_ptr<FriendInfo> friend_info)
{
    beginInsertRows(QModelIndex(), users_.size(), users_.size());
    users_.append(friend_info);
    endInsertRows();
    emit countChanged();
}

void UserListModel::clearUsers()
{
    beginResetModel();
    users_.clear();
    endResetModel();
    emit countChanged();
}

bool UserListModel::getOnline(int uid) const
{
    for (const auto &user : users_)
    {
        if (user->uid == uid)
            return user->is_online;
    }
    return false;
}

QString UserListModel::getAvatar(int uid) const
{
    for (const auto &user : users_)
    {
        if (user->uid == uid)
            return user->icon;
    }
    return {};
}

QString UserListModel::getName(int uid) const
{
    for (const auto &user : users_)
    {
        if (user->uid == uid)
            return user->name;
    }
    return {};
}

void UserListModel::removeUser(int uid)
{
    for (int i = 0; i < users_.size(); ++i)
    {
        if (users_.at(i)->uid == uid)
        {
            beginRemoveRows(QModelIndex(), i, i);
            users_.removeAt(i);
            endRemoveRows();
            emit countChanged();
            return;
        }
    }
}

int UserListModel::count() const
{
    return users_.size();
}

const QList<std::shared_ptr<FriendInfo>> &UserListModel::getUsers() const
{
    return users_;
}
