#ifndef _C4_YML_WRITER_HPP_
#define _C4_YML_WRITER_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "./common.hpp"
#endif

#include <c4/substr.hpp>
#include <stdio.h>  // fwrite(), fputc()
#include <string.h> // memcpy()


namespace c4 {
namespace yml {

/** @addtogroup doc_emit
 * @{
 */

/** @defgroup doc_writers Writer objects to use with an Emitter
 * @see Emitter
 * @{
 */


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** A writer that outputs to a file. Defaults to stdout. */
struct WriterFile
{
    FILE * m_file;
    size_t m_pos;

    WriterFile(FILE *f = nullptr) : m_file(f ? f : stdout), m_pos(0) {}

    inline substr _get(bool /*error_on_excess*/)
    {
        substr sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    template<size_t N>
    inline void _do_write(const char (&a)[N])
    {
        fwrite(a, sizeof(char), N - 1, m_file);
        m_pos += N - 1;
    }

    inline void _do_write(csubstr sp)
    {
        #if defined(__clang__)
        #   pragma clang diagnostic push
        #   pragma GCC diagnostic ignored "-Wsign-conversion"
        #elif defined(__GNUC__)
        #   pragma GCC diagnostic push
        #   pragma GCC diagnostic ignored "-Wsign-conversion"
        #endif
        if(sp.empty()) return;
        fwrite(sp.str, sizeof(csubstr::char_type), sp.len, m_file);
        m_pos += sp.len;
        #if defined(__clang__)
        #   pragma clang diagnostic pop
        #elif defined(__GNUC__)
        #   pragma GCC diagnostic pop
        #endif
    }

    inline void _do_write(const char c)
    {
        fputc(c, m_file);
        ++m_pos;
    }

    inline void _do_write(const char c, size_t num_times)
    {
        for(size_t i = 0; i < num_times; ++i)
            fputc(c, m_file);
        m_pos += num_times;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** A writer that outputs to an STL-like ostream. */
template<class OStream>
struct WriterOStream
{
    OStream& m_stream;
    size_t   m_pos;

    WriterOStream(OStream &s) : m_stream(s), m_pos(0) {}

    inline substr _get(bool /*error_on_excess*/)
    {
        substr sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    template<size_t N>
    inline void _do_write(const char (&a)[N])
    {
        m_stream.write(a, N - 1);
        m_pos += N - 1;
    }

    inline void _do_write(csubstr sp)
    {
        #if defined(__clang__)
        #   pragma clang diagnostic push
        #   pragma GCC diagnostic ignored "-Wsign-conversion"
        #elif defined(__GNUC__)
        #   pragma GCC diagnostic push
        #   pragma GCC diagnostic ignored "-Wsign-conversion"
        #endif
        if(sp.empty()) return;
        m_stream.write(sp.str, sp.len);
        m_pos += sp.len;
        #if defined(__clang__)
        #   pragma clang diagnostic pop
        #elif defined(__GNUC__)
        #   pragma GCC diagnostic pop
        #endif
    }

    inline void _do_write(const char c)
    {
        m_stream.put(c);
        ++m_pos;
    }

    inline void _do_write(const char c, size_t num_times)
    {
        for(size_t i = 0; i < num_times; ++i)
            m_stream.put(c);
        m_pos += num_times;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** a writer to a substr */
struct WriterBuf
{
    substr m_buf;
    size_t m_pos;

    WriterBuf(substr sp) : m_buf(sp), m_pos(0) {}

    inline substr _get(bool error_on_excess)
    {
        if(m_pos <= m_buf.len)
        {
            return m_buf.first(m_pos);
        }
        if(error_on_excess)
        {
            c4::yml::error("not enough space in the given buffer");
        }
        substr sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    template<size_t N>
    inline void _do_write(const char (&a)[N])
    {
        RYML_ASSERT( ! m_buf.overlaps(a));
        if(m_pos + N-1 <= m_buf.len)
        {
            memcpy(&(m_buf[m_pos]), a, N-1);
        }
        m_pos += N-1;
    }

    inline void _do_write(csubstr sp)
    {
        if(sp.empty()) return;
        RYML_ASSERT( ! sp.overlaps(m_buf));
        if(m_pos + sp.len <= m_buf.len)
        {
            memcpy(&(m_buf[m_pos]), sp.str, sp.len);
        }
        m_pos += sp.len;
    }

    inline void _do_write(const char c)
    {
        if(m_pos + 1 <= m_buf.len)
            m_buf[m_pos] = c;
        ++m_pos;
    }

    inline void _do_write(const char c, size_t num_times)
    {
        if(m_pos + num_times <= m_buf.len)
            for(size_t i = 0; i < num_times; ++i)
                m_buf[m_pos + i] = c;
        m_pos += num_times;
    }
};

/** @ } */

/** @ } */


} // namespace yml
} // namespace c4

#endif /* _C4_YML_WRITER_HPP_ */
