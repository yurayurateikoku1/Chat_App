#pragma once
#include <QString>

///@brief 通用数据结构，不同 model 按需使用其中的字段
struct UserItem
{
    int uid = 0;                // 用户id
    QString name;               // 用户名称
    QString avatar_source;      // 头像资源路径
    bool online = false;        // 是否在线
    bool is_friend = false;      // 是否是好友
    QString last_message;       // 最后一条消息
    QString time;               // 最后消息时间
    int unread_count = 0;       // 未读消息数量
};
