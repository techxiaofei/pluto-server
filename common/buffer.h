#ifndef __PLUTO_BUFFER_H__
#define __PLUTO_BUFFER_H__

#include "util.h"

namespace pluto
{

class Buffer
{
public:
	Buffer();
	~Buffer();
	uchar_t*  GetBuffer() const { return m_buffer; }
	uint32_t GetAllocSize() const { return m_alloc_size; }
	uint32_t GetWriteOffset() const { return m_write_offset; }
	void IncWriteOffset(uint32_t len) { m_write_offset += len; }

	void Extend(uint32_t len);
	uint32_t Write(void* buf, uint32_t len);
	uint32_t Read(void* buf, uint32_t len);
private:
	uchar_t*	m_buffer;
	uint32_t	m_alloc_size;
	uint32_t	m_write_offset;
};

}
#endif