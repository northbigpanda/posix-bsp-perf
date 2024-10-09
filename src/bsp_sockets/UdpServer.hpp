#ifndef __UDP_SERVER_HPP__
#define __UDP_SERVER_HPP__

#include "impl/ISocketHelper.hpp"
#include "impl/MsgDispatcher.hpp"

#include "IEventLoop.hpp"
#include <shared/BspLogger.hpp>
#include <shared/ArgParser.hpp>

#include <netinet/in.h>

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <atomic>

namespace bsp_sockets
{

using namespace bsp_perf::shared;
using UdpServerParams = struct UdpServerParams
{
    std::string ip_addr{""};
    int port{-1};
};

class UdpServer: public ISocketHelper, public std::enable_shared_from_this<UdpServer>
{
public:
    UdpServer(std::shared_ptr<IEventLoop> loop, ArgParser&& args);

    virtual ~UdpServer();

    UdpServer(const UdpServer&) = delete;
    UdpServer& operator=(const UdpServer&) = delete;
    UdpServer(UdpServer&&) = delete;
    UdpServer& operator=(UdpServer&&) = delete;

    void startLoop();

    void stop();

    UdpServerParams& getUdpServerParams() { return m_server_params; }

    void addMsgCallback(std::string& cmd_name, msgCallback msg_cb, std::any usr_data)
    { m_msg_dispatcher.addMsgCallback(cmd_name, msg_cb, usr_data); }

    std::shared_ptr<IEventLoop> getEventLoop() { return m_loop; }

    void handleRead();

    virtual int sendData(size_t cmd_id, std::vector<uint8_t>& data) override;

    using ISocketHelper::sendData;

    virtual int getFd() override { return m_sockfd; }

private:
    UdpServerParams m_server_params{};
    std::unique_ptr<BspLogger> m_logger{nullptr};
    ArgParser m_args;

    int m_sockfd{-1};
    std::shared_ptr<IEventLoop> m_loop{nullptr};

    struct sockaddr m_latest_client_addr{};

    MsgDispatcher m_msg_dispatcher{};

    std::vector<uint8_t> m_rbuf{};
    std::vector<uint8_t> m_wbuf{};

    std::atomic_bool m_running{false};
};

} // namespace bsp_sockets

#endif
