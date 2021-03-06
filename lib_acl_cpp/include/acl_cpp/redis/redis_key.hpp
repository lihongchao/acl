#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl {

class redis_result;

// redis 服务支持的数据类型分类
typedef enum
{
	REDIS_KEY_UNKNOWN,
	REDIS_KEY_NONE,
	REDIS_KEY_STRING,	// string
	REDIS_KEY_HASH,		// hash
	REDIS_KEY_LIST,		// list
	REDIS_KEY_SET,		// set
	REDIS_KEY_ZSET		// sorted set
} redis_key_t;

class ACL_CPP_API redis_key : public redis_command
{
public:
	redis_key(redis_client* conn = NULL);
	~redis_key();

	/**
	 * 删除一组 KEY，对于变参的接口，则要求最后一个参数必须以 NULL 结束
	 * @return {int} 返回所删除的 KEY 的个数，如下：
	 *  0: 未删除任何 KEY
	 *  -1: 出错
	 *  >0: 真正删除的 KEY 的个数，该值有可能少于输入的 KEY 的个数
	 */
	int del(const char* first_key, ...) ACL_CPP_PRINTF(2, 3);
	int del(const std::vector<string>& keys);
	int del(const std::vector<char*>& keys);
	int del(const std::vector<const char*>& keys);
	int del(const std::vector<int>& keys);
	int del(const char* keys[], size_t argc);
	int del(const int keys[], size_t argc);
	int del(const char* keys[], const size_t lens[], size_t argc);

	/**
	 * 序列化给定 key ，并返回被序列化的值，使用 RESTORE 命令可以将这个值反序列化
	 * 为 Redis 键
	 * @param key {const char*} 键值
	 * @param out {string&} 存储序列化的二进制数据
	 * @return {int} 序列化后数据长度
	 */
	int dump(const char* key, string& out);

	/**
	 * 判断 KEY 是否存在
	 * @param key {const char*} KEY 值
	 * @return {bool} 返回 true 表示存在，否则表示出错或不存在
	 */
	bool exists(const char* key);

	/**
	 * 设置 KEY 的生存周期，单位（秒）
	 * @param key {const char*} 键值
	 * @param n {int} 生存周期（秒）
	 * @return {int} 返回值含义如下：
	 *  > 0: 成功设置了生存周期
	 *  0：该 key 不存在
	 *  < 0: 出错
	 */
	int set_expire(const char* key, int n);

	/**
	 * 查找所有符合给定模式 pattern 的 key
	 * @param pattern {const char*} 匹配模式
	 * @param out {std::vector<string>&} 存储结果集
	 * @return {int} 结果集的数量，0--为空，<0 -- 表示出错
	 *  匹配模式举例：
	 *   KEYS * 匹配数据库中所有 key 。
	 *   KEYS h?llo 匹配 hello ， hallo 和 hxllo 等。
	 *   KEYS h*llo 匹配 hllo 和 heeeeello 等。
	 *   KEYS h[ae]llo 匹配 hello 和 hallo ，但不匹配 hillo 。
	 */
	int keys_pattern(const char* pattern, std::vector<string>& out);

	/**
	 * 将 key 改名为 newkey
	 * @return {bool}
	 */
	bool rename_key(const char* key, const char* newkey);

	/**
	 * 当且仅当 newkey 不存在时，将 key 改名为 newkey
	 * @param key {const char*} 旧 key
	 * @param newkey {const char*} 新 key
	 * @return {bool} 是否成功
	 */
	bool renamenx(const char* key, const char* newkey);

	/**
	 * 反序列化给定的序列化值，并将它和给定的 key 关联
	 * @param ttl {int} 以毫秒为单位为 key 设置生存时间，如果 ttl 为 0，
	 *  那么不设置生存时间
	 * @return {bool}
	 */
	bool restore(const char* key, const char* value, size_t len,
		int ttl, bool replace = false);

	/**
	 * 获得 KEY 的剩余生存周期，单位（秒）
	 * @param key {const char*} 键值
	 * @return {int} 返回对应键值的生存周期
	 *  > 0: 该 key 剩余的生存周期（秒）
	 * -3：出错
	 * -2：key 不存在
	 * -1：当 key 存在但没有设置剩余时间
	 * 注：对于 redis-server 2.8 以前版本，key 不存在或存在但未设置生存期则返回 -1
	 */
	int get_ttl(const char* key);

	/**
	 * 获得 KEY 的存储类型
	 * @para key {const char*} KEY 值
	 * @return {redis_key_t} 返回 KEY 的存储类型
	 */
	redis_key_t type(const char* key);

	/**
	 * 将数据从一个 redis-server 迁移至另一个 redis-server
	 * @param key {const char*} 数据对应的键值
	 * @param addr {const char*} 目标 redis-server 服务器地址，格式：ip:port
	 * @param dest_db {unsigned} 目标 redis-server 服务器的数据库 ID 号
	 * @param timeout {unsigned} 迁移过程的超时时间(毫秒级)
	 * @param option {const char*} COPY 或 REPLACE
	 * @return {bool} 迁移是否成功
	 */
	bool migrate(const char* key, const char* addr, unsigned dest_db,
		unsigned timeout, const char* option = NULL);

	/**
	 * 将数据移至本 redis-server 中的另一个数据库中
	 * @param key {const char*} 数据键值
	 * @param dest_db {unsigned} 目标数据库 ID 号
	 * @return {int} 迁移是否成功。-1: 表示出错，0：迁移失败，因为目标数据库中存在
	 *  相同键值，1：迁移成功
	 */
	int move(const char* key, unsigned dest_db);
};

} // namespace acl
