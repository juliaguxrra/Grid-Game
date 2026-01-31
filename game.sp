{
    IGNORE_LIBRARY_LEAKS
    Memcheck:Leak
    fun:*alloc
    obj:*/lib*.so.*
}