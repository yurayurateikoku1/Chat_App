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
        return user.uid;
    case NameRole:
        return user.name;
    case AvatarSourceRole:
        return user.avatar_source;
    case OnlineRole:
        return user.online;
    case IsFriendRole:
        return user.is_friend;
    }
    return {};
}

QHash<int, QByteArray> UserListModel::roleNames() const
{
    return {
        {UidRole, "uid"},
        {NameRole, "name"},
        {AvatarSourceRole, "avatarSource"},
        {OnlineRole, "online"},
        {IsFriendRole, "isFriend"}};
}

void UserListModel::addUser(int uid, const QString &name, const QString &avatar_source,
                            bool online, bool is_friend)
{
    beginInsertRows(QModelIndex(), users_.size(), users_.size());
    users_.append({uid, name, avatar_source, online, is_friend});
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
        if (user.uid == uid)
            return user.online;
    }
    return false;
}

QString UserListModel::getAvatar(int uid) const
{
    for (const auto &user : users_)
    {
        if (user.uid == uid)
            return user.avatar_source;
    }
    return {};
}

QString UserListModel::getName(int uid) const
{
    for (const auto &user : users_)
    {
        if (user.uid == uid)
            return user.name;
    }
    return {};
}

int UserListModel::count() const
{
    return users_.size();
}

const QList<UserItem> &UserListModel::getUsers() const
{
    return users_;
}
