#include "commands.h"


void* server_communication(void *param)
{
	int16_t result;
	char buffer[BUFFER];

	Threads *threads = (Threads*)param;

	while(true)
	{
		strncpy(buffer, "", BUFFER);

		result = SSL_read(core_variables.ssl, buffer, BUFFER);

		if(result <= 0 || !strcmp(buffer, DISCONNECT))
		{
			printf("\n\033[35m[Server] is demanding this client disconnection. Stopping now.\033[0m");
			pthread_cancel(*(threads->cthread));
			pthread_exit(NULL);
		}

		else
		{
			/* We received from server a non-null string, let's print it */
			if(strcmp(buffer, ""))
			{
				printf("\n\n\033[35m[Server] %s\033[0m\n\n", buffer);
				command_cursor();
			}
		}
	}
}


void* client_communication(void *param)
{
	Threads *threads = (Threads*)param;

	char buffer[BUFFER];
	char *args[BUFFER];
	int16_t nbArgs = -1;

	while(true)
	{
		command_cursor();
		prompt_keyboard(buffer);
		free_args(args, &nbArgs);
		parse_command(buffer, args, &nbArgs);

		// In order to avoid a segmentation fault if 'args' is empty...
		if(nbArgs == 0)
		{
			continue;
		}

		if(command_validation((const char* const*)args, nbArgs, EXIT, 1))
		{
			pthread_cancel(*(threads->sthread));
			pthread_exit(NULL);
		}

		else if(str_beginwith(buffer, SEND) && nbArgs >= 3)
		{
			SSL_write(core_variables.ssl, buffer, BUFFER);
		}

		else if(str_beginwith(buffer, WHISPER) && nbArgs >= 3)
		{
			SSL_write(core_variables.ssl, buffer, BUFFER);
		}

		else if(command_validation((const char* const*)args, nbArgs, WHO, 1))
		{
			char temp[BUFFER] = WHO;
			SSL_write(core_variables.ssl, temp, BUFFER);
		}

		else if(command_validation((const char* const*)args, nbArgs, CLEAR, 1))
		{
			clear_console();
		}

		else if(command_validation((const char* const*)args, nbArgs, HELP, 1))
		{
			static const char *const helpMenu[32] =
			{
				"help",
				"who",
				"send <message>",
				"whisper <idClient> <message>",
				"exit",
				"clear",
				""
			};

			for(uint8_t i = 0; helpMenu[i] != NULL; i++)
			{
				printf("\t%s\n", helpMenu[i]);
			}
		}

		else
		{
			printf("\n\033[35mUnknown command. Try \"help\" for further information.\033[0m\n\n");
		}
	}
}
