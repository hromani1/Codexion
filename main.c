/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 19:45:02 by hromani           #+#    #+#             */
/*   Updated: 2026/11/14 16:41:45 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	start_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]))
			return (error("Couldn't create thread"));
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim))
		return (error("Couldn't create thread"));
	return (1);
}

void	join_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	sim;

	if (!parse(argc, argv, &config))
		return (1);
	if (!init_simulation(&sim, &config))
		return (clean_simulation(&sim));
	if (!start_threads(&sim))
		return (clean_simulation(&sim));
	join_threads(&sim);
	clean_simulation(&sim);
	return (1);
}
