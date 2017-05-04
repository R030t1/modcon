#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>

#define handle_error(m) \
	do { perror(m); exit(EXIT_FAILURE); } while (0);

// Possible invocations:
//   `modcon r <addr>` => <value>
//   `modcon r <addr> <value> [<value>]*`
//   `modcon c <addr>` => <value>
//   `modcon c <addr> <value> [<value>]*`

int
main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t reg[64],
			 loc = 0;
			 val = 0;
	
	mb = modbus_new_tcp(argv[1], MODBUS_TCP_DEFAULT_PORT);
	if (!mb)
		handle_error("modbus_new_tcp");

	if (modbus_connect(mb) == -1) {
		modbus_free(mb);
		handle_error("modbus_connect");
	}

	// TODO: Parse arguments for the various instruction types.
	loc = atoi(argv[2]);
	val = atoi(argv[3]);
	//if (modbus_write_register(mb, 0, val) == -1) {
	if (modbus_write_bit(mb, 200, val) == -1) {
		modbus_close(mb);
		modbus_free(mb);
		handle_error("modbus_write_bit");
	}

	modbus_close(mb);
	modbus_free(mb);

	return EXIT_SUCCESS;
}
}
