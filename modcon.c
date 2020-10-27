#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <modbus/modbus.h>

#define handle_error(m) \
	do { perror(m); exit(EXIT_FAILURE); } while (0);

// Possible invocations:
//   `modcon <host> r <addr>` => <value>
//   `modcon <host> r <addr> [<value>]+`
//   `modcon <host> c <addr>` => <value>
//   `modcon <host> c <addr> [<value>]+`

enum mode { REGISTER, COIL, ERROR };
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
	char *host, *portstr;
    int port = 0;
	enum mode type;
	enum operation op;
	int addr = 0;
	uint16_t regs[64] = {0};
	uint8_t coils[64] = {0};
	modbus_t *mb;

	if (argc < 3)
		print_help();
	
    char *sep = NULL;
    host = argv[1];
    if ((sep = strchr(host, ':')))
    {
        *sep = 0;
        portstr = sep + 1;
        port = atoi(portstr);
    }
    
    if (!port)
    {
        port = MODBUS_TCP_DEFAULT_PORT;
    }
        
	if (argv[2][0] == 'r')
		type = REGISTER;
	else if (argv[2][0] == 'c')
		type = COIL;
	else if (argv[2][0] == 'e')
		type = ERROR;
	else
		print_help();
	
	if (type != ERROR && argc < 4)
		print_help();
	else if (argc > 3)
		addr = atoi(argv[3]);
	
	if (argc > 4) {
		op = WRITE;
		regs[0] = atoi(argv[4]);
		coils[0] = atoi(argv[4]) ? 1 : 0;
	}
	else op = READ;
	
	//printf("host : \"%s\"\n", host);
	//printf("type : %s\n", type == REGISTER ? "register" : "coil");
	//printf("addr : %i\n", addr);
	//printf("regs : %i\n", regs[0]);
	
	mb = modbus_new_tcp(host, port);
	if (!mb)
		handle_error("modbus_new_tcp");

	if (modbus_connect(mb) == -1) {
		modbus_free(mb);
		handle_error("modbus_connect");
	}

	if (type == REGISTER) {
		if (op == READ) {
			int rc = modbus_read_registers(mb, addr, 1, regs);
			if (rc == -1) {
				modbus_close(mb);
				modbus_free(mb);
				printf("%s\n", modbus_strerror(errno));
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
	else if (type == ERROR) {
		uint8_t raw[] = {0xFF, 0x07};
		uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
		int n;
		
		printf("Error read.\n");
		n = modbus_send_raw_request(mb, raw, sizeof(raw));
		if (n == -1) {
			modbus_close(mb);
			modbus_free(mb);
			handle_error("modbus_send_raw_request");
		}
		
		n = modbus_receive_confirmation(mb, rsp);
		if (n == -1) {
			modbus_close(mb);
			modbus_free(mb);
			handle_error("modbus_receive_confirmation");
		}
		
		for(int i = 0; i < n; i++)
			printf("%x\n", rsp[i]);
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
