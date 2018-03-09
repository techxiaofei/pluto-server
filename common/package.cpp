#include "package.h"
#include "stream.h"
#include "logger.h"

namespace pluto
{

Package::Package():buffer_(NULL)
{
	pdu_header_.command_id = 0;
    pdu_header_.sub_id = 0;
}

Package::~Package()
{
	//LogDebug("Package::~Package\n");
	if (buffer_)
	{
		delete buffer_;
		buffer_ = NULL;
	}
	
}

void Package::SetMsg(uint16_t command_id, uint16_t sub_id, char* buf)
{
	uint32_t len = strlen(buf);
	uint32_t total_length = PACKAGE_HEADER_LEN + len;
	//LogDebug("Package:SetMsg:::%d,%d\n",total_length,len);
	buffer_ = (uchar_t*)malloc(total_length + 1);

	CByteStream::WriteInt32(buffer_, total_length);
	CByteStream::WriteUint16(buffer_ + 4, command_id);
    CByteStream::WriteUint16(buffer_ + 6, sub_id);

	//uint32_t msg_size = msg.length() + 1;
	//uchar_t* sz = new uchar_t[len];

    //memcpy((char*)sz, (const char*)buf, len);  //TODO,try to reduce this copy operation

	//delete []sz;

    memcpy(buffer_+PACKAGE_HEADER_LEN, buf, len);

    pdu_header_.length = total_length;
	pdu_header_.command_id = command_id;
	pdu_header_.sub_id = sub_id;
}

int Package::ReadPduHeader(uchar_t* buf, uint32_t len)
{
	int ret = -1;
	if (len >= PACKAGE_HEADER_LEN && buf) {
		CByteStream is(buf, len);

		is >> pdu_header_.length;
		is >> pdu_header_.command_id;
        is >> pdu_header_.sub_id;

		ret = 0;
	}

	return ret;
}

bool Package::IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len)
{
	if (len < PACKAGE_HEADER_LEN)
		return false;

	pdu_len = CByteStream::ReadUint32(buf);

    //LogDebug("Package::IsPduAvailable: %d, %d\n", pdu_len, len);

	if (pdu_len > len)
	{
		//log("pdu_len=%d, len=%d\n", pdu_len, len);
		return false;
	}
    
    if(0 == pdu_len)
    {
        return false;
    }

	return true;
}

Package* Package::ReadPdu(uchar_t *buf, uint32_t len)
{
	uint32_t pdu_len = 0;
	if (!IsPduAvailable(buf, len, pdu_len))
		return NULL;

	Package* pPdu = new Package();
	pPdu->ReadPduHeader(buf, PACKAGE_HEADER_LEN);  //写入header
	//LogDebug("Package::ReadPdu length:%d, %d, %d\n", pPdu->GetTotalLength(), len, pdu_len);

	uchar_t* buffer = (uchar_t*)malloc(pdu_len+1); //TODO, add by majianfei
	memset(buffer, 0, pdu_len+1);

	memcpy(buffer, buf, pdu_len);
	pPdu->SetBuffer(buffer);

	//uint32_t i = strlen(buffer);

    //LogDebug("Package::ReadPdu:%d,%s\n",i,pPdu->GetBodyData());
    return pPdu;
}

}