#include <core/dispatchers.h>

namespace xpe {

    namespace core {

        MainDispatcher::~MainDispatcher() {}

        void MainDispatcher::submitTask(PxBaseTask& pxTask)
        {
            Dispatch({ [&pxTask]() {
                pxTask.run();
                pxTask.release();
            }});
        }

        uint32_t MainDispatcher::getWorkerCount() const
        {
            return GetWorkerCount();
        }

    }

}