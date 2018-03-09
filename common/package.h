#ifndef __PLUTO_PACKAGE_H_
#define __PLUTO_PACKAGE_H_

#include <string>
#include "util.h"
#include "buffer.h"


#define PACKAGE_HEADER_LEN		8

namespace pluto
{

//////////////////////////////
typedef struct {
    uint32_t 	length;		  // the whole pdu length
    uint16_t	command_id;	  //
    uint16_t	sub_id;	  //
} PduHeader_t;

/*
对package的操作应该有两种：
1.从socket fd里面读出到channel.InBuffer后，读package给逻辑
2.逻辑生成package，写入channel.OutBuffer;
 */
class Package
{
public:
	Package();
	~Package();

    uint32_t GetTotalLength() const {return pdu_header_.length; }
    uint16_t GetCommandId() const { return pdu_header_.command_id; }
    uint16_t GetSubId() const { return pdu_header_.sub_id; }
    //uint32_t GetBodyLength() { return strlen(body_);}
    uchar_t* GetBodyData() {return buffer_ + PACKAGE_HEADER_LEN; }
    uchar_t* GetBuffer() {return buffer_;}
    //单独set应该是没用的
    //void SetCommandId(uint16_t command_id);
    //void SetSubId(uint16_t sub_id);
    void SetBuffer(uchar_t* buffer){ buffer_ = buffer ;}
    
    //void Write(uchar_t* buf, uint32_t len) { buf_.Write((void*)buf, len); }
    //void WriteHeader();

    //写package的时候要用的
    void SetMsg(uint16_t command_id, uint16_t sub_id, char* buf);
    
    //读package的时候要用的
    int ReadPduHeader(uchar_t* buf, uint32_t len);
    static bool IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len);
    static Package* ReadPdu(uchar_t* buf, uint32_t len);

private:
    //Buffer buf_;  //recv or send，the buffer is constant
    uchar_t* buffer_;  //包体
	PduHeader_t pdu_header_;
};

}
#endif