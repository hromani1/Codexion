#include "codexion.h"


int	main(int argc, char **argv)
{
	t_config	config;


	if (!parse(argc, argv, &config))
		return (0);
}