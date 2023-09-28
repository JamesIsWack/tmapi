#include <stdio.h>

#include "Client.h"
#include "libsn.h"

int main()
{
	int ret;
	Client client;
	ret = client.Init();

	if (ret != 0)
	{
		fprintf(stderr, "client.Init failed.\n");
		return 1;
	}

	while (client.Running())
	{
		client.Update();
		client.Render();

		snSafeLoadPoint();
	}
	return 0;
}
