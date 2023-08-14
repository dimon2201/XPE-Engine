namespace xpe {

    namespace core {

        void* Object::operator new(usize byteSize)
        {
            return alloc(byteSize);
        }

        void Object::operator delete(void* address)
        {
            return dealloc(address);
        }

        void* HotObject::operator new(usize byteSize)
        {
            return halloc(byteSize);
        }

        void HotObject::operator delete(void* address)
        {
            return dehalloc(address);
        }

    }

}