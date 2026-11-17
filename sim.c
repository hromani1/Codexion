/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 22:00:08 by hromani           #+#    #+#             */
/*   Updated: 2026/11/15 17:14:05 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1)
			% sim->config.number_of_coders];
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].sim = sim;
		if (pthread_mutex_init(&sim->coders[i].lock, NULL))
			return (error("mutex initialization incomplete!"));
		i++;
	}
	return (1);
}

int	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (!heap_init(&sim->dongles[i].queue,
				2, sim->config.scheduler))
			return (error("heap initialization incomplete"));
		if (pthread_mutex_init(&sim->dongles[i].lock, NULL))
			return (error("mutex initialization incomplete!"));
		if (pthread_cond_init(&sim->dongles[i].cond, NULL))
			return (error("cond initialization incomplete"));
		sim->dongles[i].in_use = 0;
		sim->dongles[i].available_at = 0;
		sim->dongles[i].id = i;
		sim->dongles_ready++;
		i++;
	}
	return (1);
}

int	init_globals(t_simulation *sim)
{
	sim->stop = 0;
	sim->seq_counter = 0;
	sim->start_time = get_timestamp_ms();
	if (pthread_mutex_init(&sim->log_mutex, NULL)
		|| pthread_mutex_init(&sim->stop_mutex, NULL)
		|| pthread_mutex_init(&sim->seq_mutex, NULL))
	{
		sim->globals_ready = 1;
		return (error("mutex initialization incomplete!"));
	}
	return (1);
}

int	init_simulation(t_simulation *sim, t_config *config)
{
	memset(sim, 0, sizeof(t_simulation));
	sim->config = *config;
	sim->dongles = ((t_dongle *)malloc(sizeof(t_dongle)
				* config->number_of_coders));
	if (!sim->dongles)
		return (error("Couldn't allocate dongles"));
	sim->coders = ((t_coder *)malloc(sizeof(t_coder)
				* config->number_of_coders));
	if (!sim->coders)
		return (error("Couldn't allocate coders"));
	if (!init_globals(sim))
		return (error("Couldn't initialize globals"));
	if (!init_coders(sim))
		return (error("Couldn't initialize coders"));
	if (!init_dongles(sim))
		return (error("Couldn't initialize dongles"));
	return (1);
}

int	clean_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->dongles_ready || i < sim->coders_ready)
	{
		if (i < sim->dongles_ready)
		{
			heap_destroy(&sim->dongles[i].queue);
			pthread_mutex_destroy(&sim->dongles[i].lock);
			pthread_cond_destroy(&sim->dongles[i].cond);
		}
		if (i < sim->coders_ready)
			pthread_mutex_destroy(&sim->coders[i].lock);
		i++;
	}
	if (sim->globals_ready)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_mutex_destroy(&sim->seq_mutex);
	}
	free(sim->coders);
	free(sim->dongles);
	return (1);
}
