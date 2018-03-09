#include "net_util.h"
#include "channel.h"
#include "package.h"
#include "logger.h"

namespace pluto
{

Channel::Channel(int fd, struct ev_io* io_watcher, EFDTYPE fdtype):fd_(fd),io_watcher_(io_watcher),fdtype_(fdtype)
{

}


Channel::~Channel()
{
	//LogDebug("Channel::~Channel\n");
	close(fd_);

	if (io_watcher_ != NULL)
	{
		delete io_watcher_;
	}
}

Package* Channel::ReadPackage()
{
	//LogDebug("Channel::ReadPackage:%d\n", in_buf_.GetWriteOffset());
	Package* pack = NULL;
	pack = Package::ReadPdu(in_buf_.GetBuffer(), in_buf_.GetWriteOffset());

	if (pack != NULL)
	{
		in_buf_.Read(NULL, pack->GetTotalLength());
	}

	return pack;
}

}