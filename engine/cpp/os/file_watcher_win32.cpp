#ifdef WINDOWS

#define _WIN32_WINNT 0x0550
#include <windows.h>

#if defined(_MSC_VER)
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#endif

// disable secure warnings
#pragma warning (disable: 4996)

namespace xpe {

    namespace os {

        struct WatchStruct final
        {
            OVERLAPPED Overlapped;
            HANDLE DirHandle;
            BYTE Buffer[32 * K_MEMORY_KIB];
            LPARAM Param;
            DWORD NotifyFilter;
            TCHAR* DirName;
            bool IsRecursive;

            FileWatchID WatchId;
            FileWatch Parent;
        };

        bool RefreshWatch(WatchStruct* pWatch, bool clear = false);

        static const std::unordered_map<DWORD, FileWatch::eAction> s_FileWatchActionTable = {
            { FILE_ACTION_ADDED, FileWatch::eAction::ADDED },
            { FILE_ACTION_REMOVED, FileWatch::eAction::DELETED },
            { FILE_ACTION_MODIFIED, FileWatch::eAction::MODIFIED },
            { FILE_ACTION_RENAMED_NEW_NAME, FileWatch::eAction::NEW_NAME },
            { FILE_ACTION_RENAMED_OLD_NAME, FileWatch::eAction::OLD_NAME }
        };

        void CALLBACK WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
        {
            TCHAR szFile[MAX_PATH];
            PFILE_NOTIFY_INFORMATION pNotify;
            WatchStruct* watchStruct = (WatchStruct*) lpOverlapped;
            size_t offset = 0;

            if (dwNumberOfBytesTransfered == 0)
                return;

            if (dwErrorCode == ERROR_SUCCESS)
            {
                do
                {
                    pNotify = (PFILE_NOTIFY_INFORMATION) &watchStruct->Buffer[offset];
                    offset += pNotify->NextEntryOffset;

#			if defined(UNICODE)
                    {
                lstrcpynW(szFile, pNotify->FileName,
                    min(MAX_PATH, pNotify->FileNameLength / sizeof(WCHAR) + 1));
            }
#			else
                    {
                        int count = WideCharToMultiByte(
                                CP_ACP, 0, pNotify->FileName,
                                (pNotify->FileNameLength - 1) / sizeof(WCHAR),
                                szFile, MAX_PATH - 1, NULL, NULL
                        );
                        szFile[count] = TEXT('\0');
                    }
#			endif

                    watchStruct->Parent.FileWatcher->DispatchFileAction(
                            &watchStruct->Parent,
                            watchStruct->WatchId,
                            watchStruct->DirName,
                            szFile,
                            s_FileWatchActionTable.at(pNotify->Action)
                    );

                } while (pNotify->NextEntryOffset != 0);
            }

            RefreshWatch(watchStruct);
        }

        bool RefreshWatch(WatchStruct* watch, bool clear)
        {
            return ReadDirectoryChangesW(
                    watch->DirHandle,
                    watch->Buffer, sizeof(watch->Buffer),
                    watch->IsRecursive,
                    watch->NotifyFilter,
                    nullptr,
                    &watch->Overlapped,
                    clear ? nullptr : WatchCallback
            ) != 0;
        }

        void DestroyWatch(WatchStruct* watchStruct)
        {
            if (watchStruct)
            {
                CancelIo(watchStruct->DirHandle);

                RefreshWatch(watchStruct, true);

                if (!HasOverlappedIoCompleted(&watchStruct->Overlapped))
                {
                    SleepEx(5, K_TRUE);
                }

                if (watchStruct->Overlapped.hEvent)
                {
                    CloseHandle(watchStruct->Overlapped.hEvent);
                }
               
                if (watchStruct->DirHandle) 
                {
                    CloseHandle(watchStruct->DirHandle);
                }

                dealloc(watchStruct->DirName);
            }
        }

        bool CreateWatchImpl(WatchStruct* watchStruct, LPCTSTR szDirectory, bool recursive, DWORD mNotifyFilter)
        {
            watchStruct->DirHandle = CreateFile(
                    szDirectory, FILE_LIST_DIRECTORY,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL
            );

            if (watchStruct->DirHandle != INVALID_HANDLE_VALUE)
            {
                watchStruct->Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                watchStruct->NotifyFilter = mNotifyFilter;
                watchStruct->IsRecursive = recursive;

                if (RefreshWatch(watchStruct)) {
                    return true;
                }

                else {
                    CloseHandle(watchStruct->Overlapped.hEvent);
                    CloseHandle(watchStruct->DirHandle);
                }
            }

            return false;
        }

        FileWatcher::FileWatcher() : m_LastWatchId(0)
        {
        }

        FileWatcher::~FileWatcher()
        {
            for(const auto& watch : m_Watches)
            {
                DestroyWatch((WatchStruct*) watch.second);
            }
            m_Watches.clear();
        }

        FileWatch* FileWatcher::CreateWatch(const string &directory, bool recursive) {
            FileWatchID watchId = ++m_LastWatchId;

            auto* watch = allocT(WatchStruct, 1);
            watch->Parent.FileWatcher = this;
            watch->WatchId = watchId;
            watch->DirName = (TCHAR*) alloc(directory.length() + 1);
            memcpy(watch->DirName, directory.c_str(), (directory.length() + 1) * sizeof(TCHAR));

            bool created = CreateWatchImpl(
                    watch,
                    directory.c_str(),
                    recursive,
                    FILE_NOTIFY_CHANGE_CREATION |
                    FILE_NOTIFY_CHANGE_SIZE |
                    FILE_NOTIFY_CHANGE_FILE_NAME |
                    FILE_NOTIFY_CHANGE_LAST_WRITE
            );

            if (!created) {
                dealloc(watch->DirName);
                dealloc(watch);
                return nullptr;
            }

            m_Watches.insert({ watchId, &watch->Parent });

            return &watch->Parent;
        }

        void FileWatcher::RemoveWatch(const string &directory) {
            for (const auto& watch : m_Watches)
            {
                WatchStruct* watchStruct = (WatchStruct*) watch.second;
                if (directory == watchStruct->DirName)
                {
                    RemoveWatch(watch.first);
                    return;
                }
            }
        }

        void FileWatcher::RemoveWatch(FileWatchID watchId) {
            auto it = m_Watches.find(watchId);

            if (it == m_Watches.end()) {
                LogWarning("Failed to remove file watch {}", watchId);
                return;
            }

            WatchStruct* watchStruct = (WatchStruct*) it->second;
            m_Watches.erase(it);

            DestroyWatch(watchStruct);
        }

        void FileWatcher::Update() {
            MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
        }

        void FileWatcher::DispatchFileAction(
                FileWatch* fileWatch,
                FileWatchID watchId,
                const string& dirname,
                const string& filename,
                FileWatch::eAction action
        ) {

            switch (action) {

                case FileWatch::eAction::OLD_NAME:
                    {
                        auto& event = fileWatch->FileOldNameEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchId, dirname, filename);
                        }
                    }
                    break;

                case FileWatch::eAction::NEW_NAME:
                    {
                        auto& event = fileWatch->FileNewNameEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchId, dirname, filename);
                        }
                    }
                    break;

                case FileWatch::eAction::ADDED:
                    {
                        auto& event = fileWatch->FileAddedEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchId, dirname, filename);
                        }
                    }
                    break;

                case FileWatch::eAction::DELETED:
                    {
                        auto& event = fileWatch->FileDeletedEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchId, dirname, filename);
                        }
                    }
                    break;

                case FileWatch::eAction::MODIFIED:
                    {
                        auto& event = fileWatch->FileModifiedEvent;
                        if (event.Thiz != nullptr || event.Function != nullptr) {
                            event.Function(event.Thiz, watchId, dirname, filename);
                        }
                    }
                    break;

            }

        }

    }

}

#endif // WINDOWS