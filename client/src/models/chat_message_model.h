#pragma once
#include <QAbstractListModel>
#include <QQmlEngine>

// 聊天消息数据结构
struct ChatMessage
{
    QString message; // 消息内容
    QString time;    // 发送时间
    bool is_self;    // 是否为自己发送的消息
};

///@brief 聊天消息列表模型，继承 QAbstractListModel 供 QML ListView 使用
class ChatMessageModel : public QAbstractListModel
{
    Q_OBJECT
QML_ELEMENT // 注册为 QML 类型

    public :
    ///@brief 自定义角色，用于 QML delegate 中通过 role 名称访问数据
    enum Roles {
        MessageRole = Qt::UserRole + 1, // 消息内容
        TimeRole,                       // 发送时间
        IsSelfRole                      // 是否为自己发送
    };

    explicit ChatMessageModel(QObject *parent = nullptr);

    ///@brief 返回消息总数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    ///@brief 根据索引和角色返回对应数据
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    ///@brief 返回角色名称映射，QML 通过这些名称绑定数据
    QHash<int, QByteArray> roleNames() const override;

    ///@brief 添加一条消息到列表末尾
    Q_INVOKABLE void addMessage(const QString &message, const QString &time, bool is_self);
    ///@brief 清空所有消息
    Q_INVOKABLE void clearMessages();

private:
    QList<ChatMessage> messages_; // 消息列表
};
