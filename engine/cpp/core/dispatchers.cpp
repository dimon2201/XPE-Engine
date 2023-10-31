#include <core/dispatchers.h>

namespace xpe {

    namespace core {

        cMainDispatcher::~cMainDispatcher() {}

        void cMainDispatcher::submitTask(PxBaseTask& pxTask)
        {
            Dispatch({ [&pxTask]() {
                pxTask.run();
                pxTask.release();
            }});
        }

        uint32_t cMainDispatcher::getWorkerCount() const
        {
            return GetWorkerCount();
        }

    }

}