
#pragma once

#include <boost/format.hpp>


namespace ipc {
namespace imp {

template<class DUPLEX, class COUNTER>
class pipeline_2 : public COUNTER, public DUPLEX
{
private:
    enum LinkType { Uplink = 0, Downlink = 1 };

    std::string GetId(const std::string & id, LinkType lt)
    {
        return boost::str(boost::format("%1%-%2%") % id % (lt^get_ordinal()));
    }

public:
    pipeline_2(const std::string & id)
        : COUNTER(id + "-count"), DUPLEX(GetId(id, Uplink), GetId(id, Downlink))
    {
    }     
};

} // namespace ipc {
} // namespace imp {
