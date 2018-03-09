#ifndef __PLUTO_STREAM_H_
#define __PLUTO_STREAM_H_

#include "util.h"
#include "buffer.h"

namespace pluto
{

class CByteStream
{
public:
	CByteStream(uchar_t* buf, uint32_t len);
	CByteStream(Buffer* buf, uint32_t pos);
	~CByteStream() {}

	uchar_t* GetBuf() { return buffer_ ? buffer_->GetBuffer() : m_pBuf; }
	uint32_t GetPos() { return m_pos; }
	uint32_t GetLen() { return m_len; }
	void Skip(uint32_t len)
	{
		m_pos += len;
		if(m_pos > m_len)
		{
			return;
		}
	}

	static int16_t ReadInt16(uchar_t* buf);
	static uint16_t ReadUint16(uchar_t* buf);
	static int32_t ReadInt32(uchar_t* buf);
	static uint32_t ReadUint32(uchar_t* buf);
	static void WriteInt16(uchar_t* buf, int16_t data);
	static void WriteUint16(uchar_t* buf, uint16_t data);
	static void WriteInt32(uchar_t* buf, int32_t data);
	static void WriteUint32(uchar_t* buf, uint32_t data);

	void operator << (int8_t data);
	void operator << (uint8_t data);
	void operator << (int16_t data);
	void operator << (uint16_t data);
	void operator << (int32_t data);
	void operator << (uint32_t data);

	void operator >> (int8_t& data);
	void operator >> (uint8_t& data);
	void operator >> (int16_t& data);
	void operator >> (uint16_t& data);
	void operator >> (int32_t& data);
	void operator >> (uint32_t& data);

	void WriteString(const char* str);
	void WriteString(const char* str, uint32_t len);
	char* ReadString(uint32_t& len);

	void WriteData(uchar_t* data, uint32_t len);
	uchar_t* ReadData(uint32_t& len);
private:
	void _WriteByte(void* buf, uint32_t len);
	void _ReadByte(void* buf, uint32_t len);
private:
	Buffer*	buffer_;
	uchar_t*		m_pBuf;
	uint32_t		m_len;
	uint32_t		m_pos;
};

}
#endif