#include "buffer.h"
#include "logger.h"

namespace pluto
{

Buffer::Buffer()
{
	m_buffer = NULL;

	m_alloc_size = 0;
	m_write_offset = 0;
}

Buffer::~Buffer()
{
	//LogDebug("Buffer::~Buffer\n");
	m_alloc_size = 0;
	m_write_offset = 0;
	if (m_buffer)
	{
		free(m_buffer);
		m_buffer = NULL;
	}
}

/*
len:需要的空余空间长度
 */
void Buffer::Extend(uint32_t len)
{
	if (false)
	{
		LogDebug("Buffer::Extend first:%d\n",len);
		m_buffer = (uchar_t*)malloc(len);
		m_alloc_size = len;
		m_write_offset = 0;
	}
	else
	{
		int before_size = m_alloc_size;
		m_alloc_size = m_write_offset + len;

		m_alloc_size = min_pow(m_alloc_size);
		//m_alloc_size += m_alloc_size >> 2;	// increase by 1/4 allocate size
		LogDebug("Buffer::Extend second:%d,%d,%d\n", m_alloc_size, m_write_offset, len);
		if (m_alloc_size > MAX_BUF_SIZE)
		{
			if (before_size = MAX_BUF_SIZE)
			{
				LogWarning("Buffer::m_alloc_size is max");
				return;
			}
			m_alloc_size = MAX_BUF_SIZE;
		}
		uchar_t* new_buf = (uchar_t*)realloc(m_buffer, m_alloc_size);
		m_buffer = new_buf;
	}

	
}

uint32_t Buffer::Write(void* buf, uint32_t len)
{
	if (m_write_offset + len > m_alloc_size)
	{
		Extend(len);
	}

	if (buf)
	{
		memcpy(m_buffer + m_write_offset, buf, len);
	}

	m_write_offset += len;

	return len;
}

uint32_t Buffer::Read(void* buf, uint32_t len)
{
	if (len > m_write_offset)
		len = m_write_offset;

	if (buf)
		memcpy(buf, m_buffer, len);

	m_write_offset -= len;
	memmove(m_buffer, m_buffer + len, m_write_offset);
	return len;
}

}