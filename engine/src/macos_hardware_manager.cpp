#ifdef MACOS

namespace xpe
{
    namespace core
    {
        void MHardware::UpdateCpuStats()
        {
            int mib[4];
            int numCPU;
            std::size_t len = sizeof(numCPU);

            /* set the mib for hw.ncpu */
            mib[0] = CTL_HW;
            mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

            /* get the number of CPUs from the system */
            sysctl(mib, 2, &numCPU, &len, NULL, 0);

            if (numCPU < 1)
            {
                mib[1] = HW_NCPU;
                sysctl(mib, 2, &numCPU, &len, NULL, 0);
                if (numCPU < 1)
                {
                    numCPU = 1;
                }
            }

            CPU.Threads = numCPU;
            CPU.Cores = numCPU / 2;
        }
    }
}

#endif // MACOS