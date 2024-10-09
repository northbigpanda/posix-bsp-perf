#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <bsp_sockets/IEventLoop.hpp>
#include "ISocketHelper.hpp"
#include <shared/BspLogger.hpp>
#include "IOBuffer.hpp"

#include <memory>

namespace bsp_sockets
{

class TcpServer;

class TcpConnection: public ISocketHelper, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(int conn_fd, std::shared_ptr<IEventLoop> loop, std::weak_ptr<TcpServer> server);

    void activate(int conn_fd, std::shared_ptr<IEventLoop> loop);

    void handleRead();

    void handleWrite();

    int sendData(size_t cmd_id, std::vector<uint8_t>& data) override;

    int getFd() override { return m_connection_fd; }

    void cleanConnection();

private:
    int m_connection_fd{-1};
    std::shared_ptr<IEventLoop> m_loop{nullptr};
    std::weak_ptr<TcpServer> m_tcp_server;
    InputBufferQueue m_inbuf_queue{};
    OutputBufferQueue m_outbuf_queue{};

    std::unique_ptr<bsp_perf::shared::BspLogger> m_logger;
};

}

#endif
