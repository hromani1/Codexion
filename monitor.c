/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/13 23:07:58 by hromani           #+#    #+#             */
/*   Updated: 2026/11/14 16:21:45 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	find_burnout(t_simulation *sim)
{
	int			i;
	long long	last_start;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].lock);
		last_start = sim->coders[i].last_compile_start;
		pthread_mutex_unlock(&sim->coders[i].lock);
		if (elapsed_time(sim) - last_start >= sim->config.time_to_burnout)
			return (sim->coders[i].id);
		i++;
	}
	return (0);
}

int	check_completion(t_simulation *sim)
{
	int	i;
	int	count;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].lock);
		count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].lock);
		if (count < sim->config.number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

void	wake_all(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].lock);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].lock);
		i++;
	}
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int				burned;

	sim = (t_simulation *)arg;
	burned = 0;
	while (!is_stopped(sim) && !check_completion(sim))
	{
		burned = find_burnout(sim);
		if (burned)
			break ;
		usleep(1000);
	}
	set_stopped(sim);
	if (burned)
		log_msg(sim, burned, "burned out");
	wake_all(sim);
	return (NULL);
}
