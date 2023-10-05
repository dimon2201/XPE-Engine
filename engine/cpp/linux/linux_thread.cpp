#ifdef LINUX

namespace xpe {

    namespace core {

        void Thread::SetFormat(const Thread::Format &format)
        {
            #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); } while (0)

			int ret;
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			size_t cpusetsize = sizeof(cpuset);

			CPU_SET(m_ID, &cpuset);

            // Set affinity
			ret = pthread_setaffinity_np(m_Thread.native_handle(), cpusetsize, &cpuset);
			if (ret != 0) {
				handle_error_en(ret, std::string(" pthread_setaffinity_np[" + std::to_string(m_ID) + ']').c_str());
            }

			// Set name
			ret = pthread_setname_np(m_Thread.native_handle(), format.Name.c_str());
            if (ret != 0) {
				handle_error_en(ret, std::string(" pthread_setname_np[" + std::to_string(m_ID) + ']').c_str());
            }

            #undef handle_error_en
        }

    }

}

#endif // LINUX