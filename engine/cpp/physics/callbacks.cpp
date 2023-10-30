#include <physics/callbacks.h>

namespace xpe {

    namespace physics {

        PhysicsErrorCallback::PhysicsErrorCallback(bool enableInfo, bool enableWarning, bool enableError)
            : EnableInfo(enableInfo), EnableWarning(enableWarning), EnableError(enableError)
        {}

        PhysicsErrorCallback::~PhysicsErrorCallback()
        {}

        void PhysicsErrorCallback::reportError(
            PxErrorCode::Enum code,
            const char* message,
            const char* file,
            int line
        ) {
            hstringstream ss;

            ss << "\n---------------------- PhysX Debugger --------------------\n\n";

            bool logInfo = false;
            bool logWarn = false;
            bool logError = false;

            if (EnableInfo) {

                switch (code) {

                    case PxErrorCode::Enum::eNO_ERROR:
                        ss << "Type: " << "NO_ERROR" << "\n";
                        logInfo = true;
                        break;

                    case PxErrorCode::Enum::eDEBUG_INFO:
                        ss << "Type: " << "DEBUG_INFO" << "\n";
                        logInfo = true;
                        break;

                    case PxErrorCode::Enum::eMASK_ALL:
                        ss << "Type: " << "MASK_ALL" << "\n";
                        logInfo = true;
                        break;

                }

            }

            if (EnableWarning) {

                switch (code) {

                    case PxErrorCode::Enum::eDEBUG_WARNING:
                        ss << "Type: " << "DEBUG_WARNING" << "\n";
                        logWarn = true;
                        break;

                    case PxErrorCode::Enum::ePERF_WARNING:
                        ss << "Type: " << "PERF_WARNING" << "\n";
                        logWarn = true;
                        break;

                }

            }

            if (EnableError) {

                switch (code) {

                    case PxErrorCode::Enum::eABORT:
                        ss << "Type: " << "ABORT" << "\n";
                        logError = true;
                        break;

                    case PxErrorCode::Enum::eINTERNAL_ERROR:
                        ss << "Type: " << "INTERNAL_ERROR" << "\n";
                        logError = true;
                        break;

                    case PxErrorCode::Enum::eINVALID_OPERATION:
                        ss << "Type: " << "INVALID_OPERATION" << "\n";
                        logError = true;
                        break;

                    case PxErrorCode::Enum::eINVALID_PARAMETER:
                        ss << "Type: " << "INVALID_PARAMETER" << "\n";
                        logError = true;
                        break;

                    case PxErrorCode::Enum::eOUT_OF_MEMORY:
                        ss << "Type: " << "OUT_OF_MEMORY" << "\n";
                        logError = true;
                        break;

                }

            }

            ss << "File: " << file << "\n";
            ss << "Line: " << line << "\n";
            ss << "Message: " << message << "\n";

            ss << "\n----------------------------------------------------------\n";

            if (logInfo) {
                LogInfo(ss.str());
            }

            else if (logWarn) {
                LogWarning(ss.str());
            }

            else if (logError) {
                LogError(ss.str());
                debug_break();
            }
        }

        void PhysicsSimulationEventCallback::onConstraintBreak(PxConstraintInfo*, PxU32)
        {
        }

        void PhysicsSimulationEventCallback::onWake(PxActor**, PxU32)
        {
        }

        void PhysicsSimulationEventCallback::onSleep(PxActor**, PxU32)
        {
        }

        void PhysicsSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
        {
            LogInfo("onContact");
        }

        void PhysicsSimulationEventCallback::onTrigger(PxTriggerPair*, PxU32)
        {
        }

        void PhysicsSimulationEventCallback::onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
        {
        }
    }

}