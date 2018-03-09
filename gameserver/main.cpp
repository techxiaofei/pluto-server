#include "game_server.h"
#include "util.h"

using namespace pluto;

int main(int argc, char* argv[])
{
	signal(SIGPIPE, SIG_IGN);

	if (argc < 2)
	{
		printf("arg num is less than 2\n");
		return 0;
	}
	if (strncmp(argv[1], "--", 2) != 0)
	{
		printf("arg should start with --\n");
		return 0;
	}

	char* server_name = argv[1]+2;
	
	GameServer server;
	server.Start(server_name, "config.json");

	return 0;
}
