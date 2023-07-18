xpe::core::uword xpe::core::Object::GetType()
{
    return ((uword*)this)[0];
}

void* xpe::core::Object::operator new(size_t byteSize)
{
    return alloc(byteSize);
}

void xpe::core::Object::operator delete(void* address)
{
    return dealloc(address);
}