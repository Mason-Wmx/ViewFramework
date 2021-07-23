#pragma once
#include "stdafx.h"
#include "Application.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT NetworkAccessManager
	{
	public:
        static NetworkAccessManager* get(Application &app);
        static void destroy();

        void Initialize();

    private:
        class Lock
        {
        public:
            Lock() { lock.try_lock(); };
            ~Lock() { lock.unlock(); }

        private:
            std::mutex lock;
        };

    private:
        NetworkAccessManager(Application &app);
        ~NetworkAccessManager();
        static NetworkAccessManager* _instance;
	};
}
