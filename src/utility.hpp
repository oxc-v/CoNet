/**
 * 工具函数 
 */

#pragma once

#include <unistd.h>
#include <sys/socket.h>

#include "socket.hpp"

namespace conet {
namespace utility {

/**
 * @brief socket设置为非阻塞
 * @param fd 
 */
void SetNonBlocking(int fd);

/**
 * @brief 接受客户端连接
 * @param fd 
 * @param sock 
 * @param ec 
 * @return -1 错误；>0 新的socket fd
 */
int Accept(int fd, tcp::Socket& sock, error::error_code& ec);

/**
 * @brief 尝试读取len长度的数据，不一定能读取len长度的数据
 * @param fd 
 * @param buf 
 * @param len
 * @param ec 
 * @return -1 错误，并置错误码；>0 读取的字节数 
 */
int ReadSome(int fd, void* buf, size_t len, error::error_code& ec);

/**
 * @brief 读取len长度的数据，如果内核缓冲区的数据不够，ec会被设置为error::would_block
 * @param fd 
 * @param buf 
 * @param len
 * @param ec 
 * @return -1 错误；>0 读取的字节数 
 */
int Read(int fd, void* buf, size_t len, error::error_code& ec);

/**
 * @brief 尝试写入len长度的数据，不一定能写入len长度的数据
 * @param fd 
 * @param buf 
 * @param len
 * @param ec 
 * @return -1 错误；>0 写入的字节数 
 */
int WriteSome(int fd, const void* buf, size_t len, error::error_code& ec);

/**
 * @brief 写入数据，一次性写入所有数据，直到缓冲区满
 * @param fd 
 * @param buf 
 * @param ec 
 * @return -1 错误，0 没有数据，>0 写入的字节数 
 */
int Write(int fd, const void* buf, size_t len, error::error_code& ec);

/**
 * @brief 设置socket选项
 * @param fd 
 * @param level 
 * @param optname 
 * @param optval 
 * @param optlen 
 * @return 
 */
bool SetSocketOption(int fd, int level, int optname, const void* optval, socklen_t optlen);

} // namespace utility
} // namespace conet