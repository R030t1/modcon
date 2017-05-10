#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>

#define handle_error(m) \
	do { perror(m); exit(EXIT_FAILURE); } while (0);

// Possible invocations:
//   `modcon <host> r <addr>` => <value>
//   `modcon <host> r <addr> [<value>]+`
//   `modcon <host> c <addr>` => <value>
//   `modcon <host> c <addr> [<value>]+`

enum mode { REGISTER, COIL };
enum operation { READ, WRITE };

void
print_help()
{
	// TODO.
	printf("modcon <host> [r|c] <addr> [value]*");
	exit(1);
}

int
main(int argc, char *argv[])
{
	char *host;
	enum mode type;
	enum operation op;
	int addr;
	uint16_t regs[64];
	uint8_t coils[64];
	modbus_t *mb;

	if (argc < 4)
		print_help();
	
	host = argv[1];
	
	if (argv[2][0] == 'r')
		type = REGISTER;
	else if (argv[2][0] == 'c')
		type = COIL;
	else
		print_help();
	
	addr = atoi(argv[3]);
	
	if (argc > 4) {
		op = WRITE;
		regs[0] = atoi(argv[4]);
		coils[0] = atoi(argv[4]) ? 1 : 0;
	}
	else op = READ;
	
	printf("host : \"%s\"\n", host);
	printf("type : %s\n", type == REGISTER ? "register" : "coil");
	printf("addr : %i\n", addr);
	printf("regs : %i\n", regs[0]);
	
	mb = modbus_new_tcp(host, MODBUS_TCP_DEFAULT_PORT);
	if (!mb)
		handle_error("modbus_new_tcp");

	if (modbus_connect(mb) == -1) {
		modbus_free(mb);
		handle_error("modbus_connect");
	}

	if (type == REGISTER) {
		if (op == READ) {
			if (modbus_read_registers(mb, addr, 1, regs) == -1) {
				modbus_close(mb);
				modbus_free(mb);
				handle_error("modbus_read_registers");
			}
			printf("%i\n", regs[0]);
		}
		else if (op == WRITE) {
			if (modbus_write_register(mb, addr, regs[0]) == -1) {
				modbus_close(mb);
				modbus_free(mb);
				handle_error("modbus_write_register");
			}
		}
	}
	else if (type == COIL) {
		if (op == READ) {
			if (modbus_read_bits(mb, addr, 1, coils) == -1) {
				modbus_close(mb);
				modbus_free(mb);
				handle_error("modbus_read_bits");
			}
			printf("%i\n", coils[0]);
		}
		else if (op == WRITE) {
			if (modbus_write_bit(mb, addr, coils[0]) == -1) {
				modbus_close(mb);
				modbus_free(mb);
				handle_error("modbus_write_bit");
			}
		}
	}
	
	//if (modbus_write_register(mb, 0, val) == -1) {
	/*if (modbus_write_bit(mb, 200, value[0]) == -1) {
		modbus_close(mb);
		modbus_free(mb);
		handle_error("modbus_write_bit");
	}*/

	modbus_close(mb);
	modbus_free(mb);

	return EXIT_SUCCESS;
}
