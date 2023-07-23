namespace xpe {

    namespace core {

        ENGINE_API u64 Hash(const string& str) {
            std::hash<string> hasher;
            return hasher(str);
        }

        ENGINE_API u64 Hash(const hstring& str) {
            std::hash<hstring> hasher;
            return hasher(str);
        }

    }

}