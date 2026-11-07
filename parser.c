/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 19:44:59 by hromani           #+#    #+#             */
/*   Updated: 2026/11/06 19:45:00 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_int_in_range(const char *str)
{
	int		i;
	long	result;

	i = 0;
	result = 0;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		result = result * 10 + (str[i] - '0');
		if (result > INT_MAX)
			return (0);
		i++;
	}
	return (1);
}

int	error(char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (0);
}

int	parse_config(char **argv, t_config *config)
{
	if (strcmp(argv[8], "fifo") == 0)
		config->scheduler = 0;
	else if (strcmp(argv[8], "edf") == 0)
		config->scheduler = 1;
	else
		return (0);
	config->number_of_coders = atoi(argv[1]);
	config->time_to_burnout = atoi(argv[2]);
	config->time_to_compile = atoi(argv[3]);
	config->time_to_debug = atoi(argv[4]);
	config->time_to_refactor = atoi(argv[5]);
	config->number_of_compiles_required = atoi(argv[6]);
	config->dongle_cooldown = atoi(argv[7]);
	return (1);
}

int	validate_config(t_config *config)
{
	if (config->number_of_coders < 1)
		return (0);
	if (config->time_to_burnout <= 0)
		return (0);
	if (config->time_to_compile <= 0)
		return (0);
	if (config->time_to_debug <= 0)
		return (0);
	if (config->time_to_refactor <= 0)
		return (0);
	if (config->number_of_compiles_required < 1)
		return (0);
	if (config->dongle_cooldown < 0)
		return (0);
	return (1);
}

int	parse(int argc, char **argv, t_config *config)
{
	int	i;

	i = 1;
	if (argc != 9)
		return (error("missing or extra parameters!"));
	while (i < 8)
	{
		if (!is_entirely_int(argv[i]) || !is_int_in_range(argv[i]))
			return (error("invalid parameter type!"));
		i++;
	}
	if (!parse_config(argv, config))
		return (error("invalid scheduler!"));
	if (!validate_config(config))
		return (error("invalid parameter values!"));
	return (1);
}
