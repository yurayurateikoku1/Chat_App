#pragma once
#include <QAbstractListModel>
#include <QQmlEngine>
#include "user_item.h"

///@brief 通用用户列表模型，用于联系人列表、搜索结果、好友申请列表
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
        NickRole,
        IconRole,
        SexRole,
        DescRole,
        OnlineRole,
        IsFriendRole,
        StatusRole
    };

    explicit UserListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    ///@brief 添加一个用户（FriendInfo）
    void addUser(std::shared_ptr<FriendInfo> friend_info);

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
    const QList<std::shared_ptr<FriendInfo>> &getUsers() const;

signals:
    void countChanged();

private:
    QList<std::shared_ptr<FriendInfo>> users_;
};
