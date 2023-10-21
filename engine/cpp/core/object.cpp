namespace xpe {

    namespace core {

        void* Object::operator new(usize byteSize)
        {
            return main_alloc(byteSize);
        }

        void Object::operator delete(void* address)
        {
            return main_free(address);
        }

        void* HotObject::operator new(usize byteSize)
        {
            return hot_alloc(byteSize);
        }

        void HotObject::operator delete(void* address)
        {
            return hot_free(address);
        }

    }

}