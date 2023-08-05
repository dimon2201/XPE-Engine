namespace xpe {

    namespace core {

        uword Object::GetType()
        {
            return ((uword*)this)[0];
        }

        void* Object::operator new(size_t byteSize)
        {
            return alloc(byteSize);
        }

        void Object::operator delete(void* address)
        {
            return dealloc(address);
        }

    }

}