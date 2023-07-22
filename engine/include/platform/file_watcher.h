#pragma once

#include <core/event.h>

namespace xpe {

    namespace platform {

        using namespace core;

        typedef unsigned long FileWatchID;

        typedef void (*FileAddedFn)(void* thiz, FileWatchID watchId, const string& dirname, const string& filename);
        template<typename T>
        static void OnFileAdded(void* const thiz, FileWatchID watchId, const string& dirname, const string& filename) {
            ((T*) thiz)->FileAdded(watchId, dirname, filename);
        }

        typedef void (*FileDeletedFn)(void* thiz, FileWatchID watchId, const string& dirname, const string& filename);
        template<typename T>
        static void OnFileDeleted(void* const thiz, FileWatchID watchId, const string& dirname, const string& filename) {
            ((T*) thiz)->FileDeleted(watchId, dirname, filename);
        }

        typedef void (*FileModifiedFn)(void* thiz, FileWatchID watchId, const string& dirname, const string& filename);
        template<typename T>
        static void OnFileModified(void* const thiz, FileWatchID watchId, const string& dirname, const string& filename) {
            ((T*) thiz)->FileModified(watchId, dirname, filename);
        }

        typedef void (*FileNewNameFn)(void* thiz, FileWatchID watchId, const string& dirname, const string& filename);
        template<typename T>
        static void OnFileNewName(void* const thiz, FileWatchID watchId, const string& dirname, const string& filename) {
            ((T*) thiz)->FileNewName(watchId, dirname, filename);
        }

        typedef void (*FileOldNameFn)(void* thiz, FileWatchID watchId, const string& dirname, const string& filename);
        template<typename T>
        static void OnFileOldName(void* const thiz, FileWatchID watchId, const string& dirname, const string& filename) {
            ((T*) thiz)->FileOldName(watchId, dirname, filename);
        }

        class FileWatcher;

        struct ENGINE_API FileWatch {

            enum class eAction {
                ADDED,
                DELETED,
                MODIFIED,
                NEW_NAME,
                OLD_NAME
            };

            FileWatcher* FileWatcher = nullptr;

            Event<FileAddedFn> FileAddedEvent;
            Event<FileDeletedFn> FileDeletedEvent;
            Event<FileModifiedFn> FileModifiedEvent;
            Event<FileNewNameFn> FileNewNameEvent;
            Event<FileOldNameFn> FileOldNameEvent;
        };

        typedef map<FileWatchID, FileWatch*> FileWatchMap;

        class ENGINE_API FileWatcher : public Object
        {

        public:
            FileWatcher();

            ~FileWatcher();

            FileWatch* CreateWatch(const string& directory, bool recursive = false);

            void RemoveWatch(const string& directory);
            void RemoveWatch(FileWatchID watchid);

            void Update();

            void DispatchFileAction(
                    FileWatch* fileWatch,
                    FileWatchID watchId,
                    const string& dirname,
                    const string& filename,
                    FileWatch::eAction action
            );

        private:
            FileWatchMap m_Watches;
            FileWatchID m_LastWatchId;
        };

    }

}