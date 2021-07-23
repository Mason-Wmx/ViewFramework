#pragma once

#include "stdafx.h"


namespace SIM {
namespace ObjectStore {


struct SIMOBJECTSTORE_EXPORT OpenFor
{
    struct SIMOBJECTSTORE_EXPORT WriteType {};
    struct SIMOBJECTSTORE_EXPORT ReadType {};

    static const WriteType Write;
    static const ReadType Read;
};


}
}