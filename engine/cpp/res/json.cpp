namespace xpe {

    namespace res {

        bool ReadJsonFile(const char* filepath, JsonObject& object)
        {
            std::ifstream file(filepath, std::ios::in | std::ios::binary);

            if (!file.is_open())
            {
                return false;
            }

            json root = json::parse(file);

            object.FromJson(root);

            return true;
        }

        bool WriteJsonFile(const char* filepath, JsonObject& object)
        {
            std::ofstream file(filepath, std::ios::out | std::ios::binary);

            if (!file.is_open())
            {
                return false;
            }

            json root;
            object.ToJson(root);

            file << root.dump();

            return true;
        }

    }

}