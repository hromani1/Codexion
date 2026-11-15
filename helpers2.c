/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/07 20:34:53 by hromani           #+#    #+#             */
/*   Updated: 2026/11/08 19:00:45 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	log_msg(t_simulation *sim, int id, char *msg)
{
	long long	time;

	pthread_mutex_lock(&sim->log_mutex);
	time = elapsed_time(sim);
	if (!is_stopped(sim))
		printf("%lld %i %s\n", time, id, msg);
	else
	{
		if (!strcmp(msg, "burned out"))
			printf("%lld %i %s\n", time, id, msg);
	}
	pthread_mutex_unlock(&sim->log_mutex);
	return (time);
}

void	precise_sleep(long ms, t_simulation *sim)
{
	long long	end;

	end = get_timestamp_ms() + ms;
	while (get_timestamp_ms() < end && !is_stopped(sim))
		usleep(500);
}
