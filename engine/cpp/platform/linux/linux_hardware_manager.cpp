#ifdef LINUX

#include <sys/sysinfo.h>

   int sysinfo(struct sysinfo *info);

   struct sysinfo {
       long uptime;             /* Seconds since boot */
       unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
       unsigned long totalram;  /* Total usable main memory size */
       unsigned long freeram;   /* Available memory size */
       unsigned long sharedram; /* Amount of shared memory */
       unsigned long bufferram; /* Memory used by buffers */
       unsigned long totalswap; /* Total swap space size */
       unsigned long freeswap;  /* swap space still available */
       unsigned short procs;    /* Number of current processes */
       unsigned long totalhigh; /* Total high memory size */
       unsigned long freehigh;  /* Available high memory size */
       unsigned int mem_unit;   /* Memory unit size in bytes */
       char _f[20-2*sizeof(long)-sizeof(int)]; /* Padding for libc5 */
   };

   namespace xpe {

       namespace core {

           void cHardwareManager::UpdateCpuStats()
           {
               CPU.Cores = sysconf(_SC_NPROCESSORS_ONLN);
           }

       }

   }

#endif // LINUX