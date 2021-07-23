
#pragma once

#include <boost/iostreams/stream.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4584) // we don't need to be warned of duplicated base classes here, it's done on purpose
#endif

namespace ipc {
namespace imp {

class arch_setup_default
{
public:
    template<class ARCHIVE>
    static void setup(ARCHIVE & arch)
    {
    }
};

template<class DEVICE>
class pipeline_link
{
public:
    pipeline_link(const std::string & linkId)
        : _device(new DEVICE(linkId))
    {
    }

    DEVICE & GetDevice()
    {
        return *_device;
    }

private:
    std::shared_ptr<DEVICE> _device;
};

template<class DEVICE>
class pipeline_link_streamed : public pipeline_link<DEVICE>
{
public:
    pipeline_link_streamed(const std::string & linkId)
        : pipeline_link<DEVICE>(linkId),
          _stream(new boost::iostreams::stream<DEVICE>(GetDevice()))
    {
    }

    boost::iostreams::stream<DEVICE> & GetStream()
    {
        return *_stream;
    }

private:
    std::shared_ptr<boost::iostreams::stream<DEVICE>> _stream;
};

template<class DEVICE, class ARCH_SETUP = arch_setup_default>
class pipeline_uplink :
    public pipeline_link<DEVICE>,
    public pipeline_link_streamed<DEVICE>
{
public:
    pipeline_uplink(const std::string & linkId)
        : pipeline_link<DEVICE>(linkId + "-raw"),
          pipeline_link_streamed<DEVICE>(linkId + "-streamed")
    {
    }

    template<class T>
    pipeline_uplink & operator<<(const T & obj)
    {
        send(obj, boost::is_pod<T>());
        return *this;
    }

private:
    // send POD using bitwise copy
    template<class T>
    void send(const T & obj, const boost::true_type&)
    {
        pipeline_link<DEVICE>::GetDevice().write(reinterpret_cast<const DEVICE::char_type*>(&obj), sizeof(T));
        pipeline_link<DEVICE>::GetDevice().set_eof();
    }

    // version for classes uses boost serialization
    template<class T>
    void send(const T & obj, const boost::false_type&)
    {
        boost::archive::binary_oarchive oa(GetStream());
        ARCH_SETUP::setup(oa);
        oa << BOOST_SERIALIZATION_NVP(obj);
        pipeline_link_streamed<DEVICE>::GetDevice().set_eof();
    }
};

template<class DEVICE, class ARCH_SETUP = arch_setup_default>
class pipeline_downlink :
    public pipeline_link<DEVICE>,
    public pipeline_link_streamed<DEVICE>
{
public:
    pipeline_downlink(const std::string & linkId)
        : pipeline_link<DEVICE>(linkId + "-raw"),
          pipeline_link_streamed<DEVICE>(linkId + "-streamed")
    {
    }

    template<class T>
    pipeline_downlink & operator>>(T & obj)
    {
        recv(obj, boost::is_pod<T>());
        return *this;
    }

private:
    // receive POD using bitwise copy
    template<class T>
    void recv(T & obj, const boost::true_type&)
    {
        pipeline_link<DEVICE>::GetDevice().read(reinterpret_cast<DEVICE::char_type*>(&obj), sizeof(T));
    }

    // version for classes uses boost serialization
    template<class T>
    void recv(T & obj, const boost::false_type&)
    {
        boost::archive::binary_iarchive ia(GetStream());
        ARCH_SETUP::setup(ia);
        ia >> BOOST_SERIALIZATION_NVP(obj);
    }
};

template<class U_DEV, class D_DEV, class ARCH_SETUP = arch_setup_default>
class pipeline_duplex : public pipeline_uplink<U_DEV, ARCH_SETUP>, public pipeline_downlink<D_DEV, ARCH_SETUP>
{
public:
    pipeline_duplex(const std::string & uplinkId, const std::string & downlinkId)
        : pipeline_uplink<U_DEV, ARCH_SETUP>(uplinkId), pipeline_downlink<D_DEV, ARCH_SETUP>(downlinkId)
    {
    }

    template<class T>
    pipeline_duplex & operator<<(const T & obj)
    {
        pipeline_uplink<U_DEV, ARCH_SETUP>::operator <<(obj);
        return *this;
    }

    template<class T>
    pipeline_duplex & operator>>(T & obj)
    {
        pipeline_downlink<D_DEV, ARCH_SETUP>::operator >>(obj);
        return *this;
    }
};

} // namespace ipc {
} // namespace imp {

#ifdef WINDOWS
#pragma warning(pop) //#pragma warning(disable: 4584)
#endif

