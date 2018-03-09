#ifndef __PLUTO_CHANNEL_H__
#define __PLUTO_CHANNEL_H__

#include <iostream>
#include <map>
#include <list>
#include <ev.h>

#include "util.h"
#include "buffer.h"

namespace pluto
{

class Package;
class EpollServer;

class Channel
{
	public:
		Channel(int fd, struct ev_io* io_watcher, EFDTYPE fdtype);
		virtual ~Channel();

		Package* ReadPackage();

		int Fd(){return fd_;}

		EFDTYPE GetFdType() {return fdtype_;}

		struct ev_io* GetIoWatcher(){return io_watcher_;}
		void SetIoWatcher(struct ev_io* io_watcher){io_watcher_ = io_watcher;}

		Buffer& InBuffer() {return in_buf_;}
		Buffer& OutBuffer() {return out_buf_;}

	protected:
		int fd_;
	private:
		struct ev_io* io_watcher_;
		EFDTYPE fdtype_;

		Buffer in_buf_;
		Buffer out_buf_;
};

}

#endif

