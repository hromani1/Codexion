/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 22:23:13 by hromani           #+#    #+#             */
/*   Updated: 2026/11/07 20:30:43 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_timestamp_ms(void)
{
	long long		timestamp;
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	timestamp = 0;
	timestamp += (long long)(tv.tv_sec * 1000);
	timestamp += (long long)(tv.tv_usec / 1000);
	return (timestamp);
}

long long	elapsed_time(t_simulation *sim)
{
	long long	current;
	long long	start;

	start = sim->start_time;
	current = get_timestamp_ms();
	return (current - start);
}

int	is_stopped(t_simulation *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	set_stopped(t_simulation *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}
