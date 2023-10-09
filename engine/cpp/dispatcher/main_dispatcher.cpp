#include <dispatcher/main_dispatcher.h>

namespace xpe {

    namespace core {

        MainDispatcher::~MainDispatcher() {}

        void MainDispatcher::submitTask(PxBaseTask& pxTask)
        {
            Task task;
            task.Runnable = [&pxTask]() {
                pxTask.run();
                pxTask.release();
            };
            Dispatch(task);
        }

        uint32_t MainDispatcher::getWorkerCount() const
        {
            return GetWorkerCount();
        }

    }

}