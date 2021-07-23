#pragma once


namespace SIM {
namespace ObjectStore {

enum class ObjectState : unsigned short { Ok, Invalid, MarkedForDelete, Deleted, Placeholder };
enum class ObjectLocks : unsigned short { None = 0, Read = 1, Write = 2 };

}
}