#include <prcsJ2735.h>

/* allocator used by the ASN.1 runtime */
void *asn1_malloc(size_t size)
{
    return malloc(size);
}

void *asn1_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void asn1_free(void *ptr)
{
    free(ptr);
}

void asn1_xer_printf(const ASN1CType *msg_type, void* msg)
{
    uint8_t *buf;
    int32_t buf_len;

    buf_len = asn1_xer_encode(&buf, msg_type, msg);

//    std::cout << buf << std::endl;
    printf("%s\n", buf);

    asn1_free(buf);

    return;
}


