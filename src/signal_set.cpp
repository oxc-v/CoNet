#include "signal_set.hpp"

#include <csignal>
#include <sys/signalfd.h>

#include "awaiter.hpp"

namespace conet {
namespace signal {

SignalSet::SignalSet(EventLoop& loop)
    : loop_(loop)
{
    sigset_t mask;
    sigemptyset(&mask);
    auto sfd = signalfd(-1, &mask, SFD_NONBLOCK);
    if (sfd == -1) {
        perror("signalfd");
        exit(1);
    }

    channel_.SetFd(sfd);
    channel_.SetEvents(EPOLLIN | EPOLLET);
    loop_.AddChannel(&channel_);
}

SignalSet::~SignalSet()
{
    channel_.Close();
    loop_.RemoveChannel(channel_.Fd());
}

void SignalSet::Add(int signo)
{
    signals_.insert(signo);
}

void SignalSet::Remove(int signo)
{
    signals_.erase(signo);
}

bool SignalSet::Contains(int signo) const
{
    return signals_.find(signo) != signals_.end();
}

void SignalSet::Clear()
{
    signals_.clear();
}

const std::unordered_set<int>& SignalSet::GetSignals() const
{
    return signals_;
}

awaiter::SignalAwaiter SignalSet::AsyncWait(error::error_code& ec)
{
    sigset_t mask;
    for (auto signo : signals_)
        sigaddset(&mask, signo);

    pthread_sigmask(SIG_BLOCK, &mask, nullptr);

    auto ret = signalfd(channel_.Fd(), &mask, 0);
    if (ret == -1)
        ec.assign(error::operator_error);

    return awaiter::SignalAwaiter(channel_, ec, *this);
}

} // namespace signal
} // namespace conet