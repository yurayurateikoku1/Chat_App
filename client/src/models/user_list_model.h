#pragma once
#include <QAbstractListModel>
#include <QQmlEngine>
#include "user_item.h"

///@brief 通用用户列表模型，用于搜索结果和联系人列表
class UserListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles
    {
        UidRole = Qt::UserRole + 1,
        NameRole,
        AvatarSourceRole,
        OnlineRole,
        IsFriendRole
    };

    explicit UserListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    ///@brief 添加一个用户
    Q_INVOKABLE void addUser(int uid, const QString &name, const QString &avatar_source = "",
                             bool online = false, bool is_friend = false);

    ///@brief 清空列表
    Q_INVOKABLE void clearUsers();

    ///@brief 根据uid查询在线状态
    Q_INVOKABLE bool getOnline(int uid) const;

    ///@brief 根据uid查询头像路径
    Q_INVOKABLE QString getAvatar(int uid) const;

    ///@brief 根据uid查询名称
    Q_INVOKABLE QString getName(int uid) const;

    ///@brief 根据uid移除用户
    Q_INVOKABLE void removeUser(int uid);

    ///@brief 获取数量
    int count() const;

    ///@brief 获取所有用户数据
    const QList<UserItem> &getUsers() const;

signals:
    void countChanged();

private:
    QList<UserItem> users_;
};
