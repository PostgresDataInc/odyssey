
/*
 * machinarium.
 *
 * Cooperative multitasking engine.
*/

#include <machinarium.h>
#include <uv.h>
#include <assert.h>

static void
test_connect(void *arg)
{
	machine_t machine = arg;
	printf("child started\n");
	machine_io_t client = machine_create_io(machine);
	struct sockaddr_in sa;
	uv_ip4_addr("8.8.8.8", 1324, &sa);
	int rc;
	rc = machine_connect(client, (struct sockaddr*)&sa, 0);
	printf("child resumed\n");
	assert(rc < 0);
	machine_close(client);
	if (machine_cancelled(machine))
		printf("child marked as cancel\n");
	printf("child end\n");
}

static void
test_waiter(void *arg)
{
	machine_t machine = arg;

	printf("waiter started\n");

	int id = machine_create_fiber(machine, test_connect, machine);
	machine_sleep(machine, 0);
	machine_cancel(machine, id);
	machine_wait(machine, id);

	printf("waiter 1 ended \n");
	machine_stop(machine);
}

int
main(int argc, char *argv[])
{
	machine_t machine = machine_create();
	machine_create_fiber(machine, test_waiter, machine);
	machine_start(machine);
	printf("shutting down\n");
	machine_free(machine);
	return 0;
}
