#pragma once
#include <QString>
#include <QMetaType>
#include <QQmlEngine>
#include <QJsonArray>
#include <QJsonObject>
/**
 * @brief 精确查找用户的结果（Q_GADGET，可直接暴露给 QML）
 */
struct SearchResult
{
    Q_GADGET
    QML_VALUE_TYPE(searchResult)
    Q_PROPERTY(int uid MEMBER uid)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString icon MEMBER icon)
    Q_PROPERTY(bool valid MEMBER valid)
public:
    int uid = -1;
    QString name;
    QString icon;
    bool valid = false;
};

/**
 * @brief 用户搜索结果信息
 *
 * 存储通过搜索功能查找到的用户基本信息。
 */
struct SearchInfo
{
public:
    /**
     * @brief 构造搜索结果信息
     * @param uid  用户ID
     * @param name 用户名
     * @param nick 昵称
     * @param desc 个人描述
     * @param sex  性别 (0: 未知, 1: 男, 2: 女)
     * @param icon 头像URL
     */
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon)
        : uid(uid), name(name), nick(nick), desc(desc), sex(sex), icon(icon) {}
    int uid;      ///< 用户ID
    QString name; ///< 用户名
    QString nick; ///< 昵称
    QString desc; ///< 个人描述
    int sex;      ///< 性别
    QString icon; ///< 头像URL
};

/**
 * @brief 添加好友申请信息
 *
 * 存储发起好友申请时的相关数据。
 */
struct AddFriendApply
{
public:
    /**
     * @brief 构造好友申请信息
     * @param fromuid 申请发起者的用户ID
     * @param name    用户名
     * @param desc    申请描述/验证消息
     * @param icon    头像URL
     * @param nick    昵称
     * @param sex     性别
     */
    AddFriendApply(int fromuid, QString name, QString desc,
                   QString icon, QString nick, int sex)
        : fromuid(fromuid), name(name), desc(desc),
          icon(icon), nick(nick), sex(sex) {}
    int fromuid;  ///< 申请发起者的用户ID
    QString name; ///< 用户名
    QString desc; ///< 申请描述/验证消息
    QString icon; ///< 头像URL
    QString nick; ///< 昵称
    int sex;      ///< 性别
};

/**
 * @brief 好友申请处理信息
 *
 * 用于展示和管理好友申请列表，包含申请的处理状态。
 */
struct ApplyInfo
{
    /**
     * @brief 构造申请信息
     * @param uid    申请者用户ID
     * @param name   用户名
     * @param desc   申请描述
     * @param icon   头像URL
     * @param nick   昵称
     * @param sex    性别
     * @param status 处理状态 (0: 待处理, 1: 已同意, 2: 已拒绝)
     */
    ApplyInfo(int uid, QString name, QString desc,
              QString icon, QString nick, int sex, int status)
        : uid(uid), name(name), desc(desc),
          icon(icon), nick(nick), sex(sex), status(status) {}

    /**
     * @brief 从好友申请信息构造，状态默认为待处理
     * @param addinfo 好友申请信息的智能指针
     */
    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        : uid(addinfo->fromuid), name(addinfo->name),
          desc(addinfo->desc), icon(addinfo->icon),
          nick(addinfo->nick), sex(addinfo->sex),
          status(0)
    {
    }

    /**
     * @brief 设置头像
     * @param head 新的头像URL
     */
    void setIcon(QString head)
    {
        icon = head;
    }
    int uid;      ///< 申请者用户ID
    QString name; ///< 用户名
    QString desc; ///< 申请描述
    QString icon; ///< 头像URL
    QString nick; ///< 昵称
    int sex;      ///< 性别
    int status;   ///< 处理状态
};

/**
 * @brief 认证请求信息
 *
 * 存储发起好友认证时的用户信息。
 */
struct AuthInfo
{
    /**
     * @brief 构造认证请求信息
     * @param uid  用户ID
     * @param name 用户名
     * @param nick 昵称
     * @param icon 头像URL
     * @param sex  性别
     */
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex) : uid(uid), name(name), nick(nick), icon(icon),
                                                    sex(sex) {}
    int uid;      ///< 用户ID
    QString name; ///< 用户名
    QString nick; ///< 昵称
    QString icon; ///< 头像URL
    int sex;      ///< 性别
};

/**
 * @brief 认证响应信息
 *
 * 存储好友认证通过后对方的用户信息。
 */
struct AuthRsp
{
    /**
     * @brief 构造认证响应信息
     * @param peeruid  对方用户ID
     * @param peername 对方用户名
     * @param peernick 对方昵称
     * @param peericon 对方头像URL
     * @param peersex  对方性别
     */
    AuthRsp(int peeruid, QString peername,
            QString peernick, QString peericon, int peersex)
        : uid(peeruid), name(peername), nick(peernick),
          icon(peericon), sex(peersex)
    {
    }

    int uid;      ///< 对方用户ID
    QString name; ///< 对方用户名
    QString nick; ///< 对方昵称
    QString icon; ///< 对方头像URL
    int sex;      ///< 对方性别
};

struct TextChatData;

/**
 * @brief 好友信息
 *
 * 存储好友的详细信息及聊天记录。
 */
struct FriendInfo
{
    /**
     * @brief 构造好友信息
     * @param uid      好友用户ID
     * @param name     用户名
     * @param nick     昵称
     * @param icon     头像URL
     * @param sex      性别
     * @param desc     个人描述
     * @param back     备注名
     * @param last_msg 最后一条消息内容
     */
    FriendInfo(int uid, QString name, QString nick, QString icon,
               int sex, QString desc, QString back, QString last_msg = "") : uid(uid),
                                                                             name(name), nick(nick), icon(icon), sex(sex), desc(desc),
                                                                             back(back), last_msg(last_msg) {}

    /**
     * @brief 从认证请求信息构造好友信息
     * @param auth_info 认证请求信息的智能指针
     */
    FriendInfo(std::shared_ptr<AuthInfo> auth_info) : uid(auth_info->uid),
                                                      nick(auth_info->nick), icon(auth_info->icon), name(auth_info->name),
                                                      sex(auth_info->sex) {}

    /**
     * @brief 从认证响应信息构造好友信息
     * @param auth_rsp 认证响应信息的智能指针
     */
    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp) : uid(auth_rsp->uid),
                                                    nick(auth_rsp->nick), icon(auth_rsp->icon), name(auth_rsp->name),
                                                    sex(auth_rsp->sex) {}

    /**
     * @brief 追加聊天消息记录
     * @param text_vec 聊天消息列表
     */
    void appendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec)
    {
        for (const auto &text : text_vec)
        {
            chat_msgs.push_back(text);
        }
    }

    int uid;                                              ///< 好友用户ID
    QString name;                                         ///< 用户名
    QString nick;                                         ///< 昵称
    QString icon;                                         ///< 头像URL
    int sex;                                              ///< 性别
    QString desc;                                         ///< 个人描述
    QString back;                                         ///< 备注名
    QString last_msg;                                     ///< 最后一条消息
    QString time;                                         ///< 最后消息时间
    bool is_online = false;                               ///< 是否在线
    bool is_friend = false;                               ///< 是否为好友
    int status = 0;                                       ///< 申请状态 (0: 待处理, 1: 已同意, 2: 已拒绝)
    int unread_cnt = 0;                                   ///< 未读消息数
    std::vector<std::shared_ptr<TextChatData>> chat_msgs; ///< 聊天消息记录
};

/**
 * @brief 用户信息
 *
 * 通用的用户信息结构，用于聊天列表、联系人展示等场景。
 * 支持从多种来源（认证信息、搜索结果、好友信息）构造。
 */
struct UserInfo
{
    /**
     * @brief 构造用户信息
     * @param uid      用户ID
     * @param name     用户名
     * @param nick     昵称
     * @param icon     头像URL
     * @param sex      性别
     * @param last_msg 最后一条消息内容
     * @param desc     个人描述
     */
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "", QString desc = "") : uid(uid), name(name), nick(nick), icon(icon), sex(sex), last_msg(last_msg), desc(desc) {}

    /**
     * @brief 从认证请求信息构造
     * @param auth 认证请求信息的智能指针
     */
    UserInfo(std::shared_ptr<AuthInfo> auth) : uid(auth->uid), name(auth->name), nick(auth->nick),
                                               icon(auth->icon), sex(auth->sex), last_msg(""), desc("") {}

    /**
     * @brief 从基本信息构造（昵称默认与用户名相同）
     * @param uid  用户ID
     * @param name 用户名
     * @param icon 头像URL
     */
    UserInfo(int uid, QString name, QString icon) : uid(uid), name(name), icon(icon), nick(name),
                                                    sex(0), last_msg(""), desc("")
    {
    }

    /**
     * @brief 从认证响应信息构造
     * @param auth 认证响应信息的智能指针
     */
    UserInfo(std::shared_ptr<AuthRsp> auth) : uid(auth->uid), name(auth->name), nick(auth->nick),
                                              icon(auth->icon), sex(auth->sex), last_msg("") {}

    /**
     * @brief 从搜索结果信息构造
     * @param search_info 搜索结果信息的智能指针
     */
    UserInfo(std::shared_ptr<SearchInfo> search_info) : uid(search_info->uid), name(search_info->name), nick(search_info->nick),
                                                        icon(search_info->icon), sex(search_info->sex), last_msg("")
    {
    }

    /**
     * @brief 从好友信息构造，同时拷贝聊天记录
     * @param friend_info 好友信息的智能指针
     */
    UserInfo(std::shared_ptr<FriendInfo> friend_info) : uid(friend_info->uid), name(friend_info->name), nick(friend_info->nick),
                                                        icon(friend_info->icon), sex(friend_info->sex), last_msg("")
    {
        chat_msgs = friend_info->chat_msgs;
    }

    int uid;                                              ///< 用户ID
    QString name;                                         ///< 用户名
    QString nick;                                         ///< 昵称
    QString icon;                                         ///< 头像URL
    int sex;                                              ///< 性别
    QString desc;                                         ///< 个人描述
    QString last_msg;                                     ///< 最后一条消息
    bool is_online = false;                               ///< 是否在线
    int unread_cnt = 0;                                   ///< 未读消息数
    std::vector<std::shared_ptr<TextChatData>> chat_msgs; ///< 聊天消息记录
};

/**
 * @brief 文本聊天数据
 *
 * 存储单条文本聊天消息的内容和收发双方信息。
 */
struct TextChatData
{
    /**
     * @brief 构造文本聊天数据
     * @param msg_id      消息ID
     * @param msg_content 消息内容
     * @param fromuid     发送者用户ID
     * @param touid       接收者用户ID
     */
    TextChatData(QString msg_id, QString msg_content, int fromuid, int touid)
        : msg_id(msg_id), msg_content(msg_content), from_uid(fromuid), to_uid(touid)
    {
    }
    QString msg_id;      ///< 消息ID
    QString msg_content; ///< 消息内容
    int from_uid;        ///< 发送者用户ID
    int to_uid;          ///< 接收者用户ID
};

/**
 * @brief 文本聊天消息集合
 *
 * 从JSON数组解析并存储一组聊天消息，用于批量处理收发的聊天数据。
 */
struct TextChatMsg
{
    /**
     * @brief 从JSON数组构造聊天消息集合
     * @param fromuid 发送者用户ID
     * @param touid   接收者用户ID
     * @param arrays  包含消息数据的JSON数组，每个元素包含 "content" 和 "msgid" 字段
     */
    TextChatMsg(int fromuid, int touid, QJsonArray arrays) : from_uid(fromuid), to_uid(touid)
    {
        for (auto msg_data : arrays)
        {
            auto msg_obj = msg_data.toObject();
            auto content = msg_obj["content"].toString();
            auto msgid = msg_obj["msgid"].toString();
            auto msg_ptr = std::make_shared<TextChatData>(msgid, content, fromuid, touid);
            chat_msgs.push_back(msg_ptr);
        }
    }
    int to_uid;                                           ///< 接收者用户ID
    int from_uid;                                         ///< 发送者用户ID
    std::vector<std::shared_ptr<TextChatData>> chat_msgs; ///< 解析后的聊天消息列表
};
