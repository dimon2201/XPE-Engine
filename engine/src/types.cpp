namespace xpe
{
    namespace core
    {
        void* cObject::operator new(usize byteSize)
        {
            return main_alloc(byteSize);
        }

        void cObject::operator delete(void* address)
        {
            return main_free(address);
        }

        void* cHotObject::operator new(usize byteSize)
        {
            return hot_alloc(byteSize);
        }

        void cHotObject::operator delete(void* address)
        {
            return hot_free(address);
        }
    }
}