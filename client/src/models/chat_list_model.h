#pragma once
#include <QAbstractListModel>
#include <QQmlEngine>
#include "user_item.h"
#include "chat_message_model.h"

///@brief 聊天会话列表模型，每个会话关联一个 ChatMessageModel
class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum Roles
    {
        UidRole = Qt::UserRole + 1,
        NameRole,
        LastMessageRole,
        TimeRole,
        AvatarSourceRole,
        UnreadCountRole
    };

    explicit ChatListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool hasMore() const;
    bool loading() const;

    ///@brief 添加一个聊天会话
    Q_INVOKABLE void addChat(int uid, const QString &name, const QString &last_message,
                             const QString &time, const QString &avatar_source = "",
                             int unread_count = 0);

    ///@brief 清空所有会话
    Q_INVOKABLE void clearChats();

    ///@brief 加载更多数据（分页）
    Q_INVOKABLE void loadMore();

    ///@brief 根据uid查找名称
    QString getNameByUid(int uid) const;

    ///@brief 是否已存在该uid的会话
    Q_INVOKABLE bool hasChat(int uid) const;

    ///@brief 获取指定uid的消息模型（不存在则创建）
    ChatMessageModel *getMessageModel(int uid);

    ///@brief 删除指定uid的会话
    Q_INVOKABLE void removeChat(int uid);

    ///@brief 未读数+1
    void incrementUnread(int uid);

    ///@brief 清零未读数
    void clearUnread(int uid);

signals:
    void hasMoreChanged();
    void loadingChanged();

private:
    struct ChatEntry
    {
        UserItem user;
        ChatMessageModel *message_model = nullptr;
    };

    QList<ChatEntry> chats_;
    bool has_more_ = true;
    bool loading_ = false;
    int page_ = 0;
    static constexpr int kPageSize = 20;
};
