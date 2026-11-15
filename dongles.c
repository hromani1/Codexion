/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 20:32:25 by hromani           #+#    #+#             */
/*   Updated: 2026/11/15 15:01:34 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	can_take(t_coder *coder, t_dongle *dongle, t_simulation *sim)
{
	if (dongle->in_use)
		return (0);
	if (elapsed_time(sim) < dongle->available_at)
		return (0);
	return (heap_peek(&dongle->queue).coder_id == coder->id);
}

void	set_deadline(struct timespec *ts, long long abs_ms)
{
	ts->tv_sec = abs_ms / 1000;
	ts->tv_nsec = (abs_ms % 1000) * 1000000;
}

int	wait_turn(t_coder *coder, t_dongle *first,
				t_dongle *second, t_simulation *sim)
{
	struct timespec	ts;

	while (!can_take(coder, first, sim) || !can_take(coder, second, sim))
	{
		if (is_stopped(sim))
		{
			heap_remove(&first->queue, coder->id);
			heap_remove(&second->queue, coder->id);
			pthread_mutex_unlock(&second->lock);
			pthread_mutex_unlock(&first->lock);
			return (0);
		}
		pthread_mutex_unlock(&second->lock);
		set_deadline(&ts, get_timestamp_ms() + 1);
		pthread_cond_timedwait(&first->cond, &first->lock, &ts);
		pthread_mutex_lock(&second->lock);
	}
	return (1);
}

int	single_coder(t_coder *coder, t_simulation *sim)
{
	log_msg(sim, coder->id, "has taken a dongle");
	return (0);
}

int	acquire_both(t_coder *coder, t_simulation *sim)
{
	t_dongle	*first;
	t_dongle	*second;
	t_request	req;

	if (coder->id % 2 == 0)
		precise_sleep(5, sim);
	if (sim->config.number_of_coders == 1)
		return (single_coder(coder, sim));
	order_dongles(coder, &first, &second);
	req = make_request(coder, sim);
	pthread_mutex_lock(&first->lock);
	pthread_mutex_lock(&second->lock);
	heap_push(&first->queue, req);
	heap_push(&second->queue, req);
	if (!wait_turn(coder, first, second, sim))
		return (0);
	heap_pop(&first->queue);
	heap_pop(&second->queue);
	first->in_use = 1;
	second->in_use = 1;
	pthread_mutex_unlock(&second->lock);
	pthread_mutex_unlock(&first->lock);
	log_msg(sim, coder->id, "has taken a dongle");
	log_msg(sim, coder->id, "has taken a dongle");
	return (1);
}
