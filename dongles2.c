/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 22:01:05 by hromani           #+#    #+#             */
/*   Updated: 2026/11/14 18:45:24 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_both_dongles(t_coder *coder, t_simulation *sim)
{
	release_dongle(coder->left, sim);
	if (sim->config.number_of_coders > 1)
		release_dongle(coder->right, sim);
}

void	release_dongle(t_dongle *dongle, t_simulation *sim)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->in_use = 0;
	dongle->available_at = elapsed_time(sim) + sim->config.dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

int	get_seq(t_simulation *sim)
{
	int	seq;

	pthread_mutex_lock(&sim->seq_mutex);
	seq = sim->seq_counter++;
	pthread_mutex_unlock(&sim->seq_mutex);
	return (seq);
}

t_request	make_request(t_coder *coder, t_simulation *sim)
{
	t_request	req;

	req.coder_id = coder->id;
	req.seq = get_seq(sim);
	if (sim->config.scheduler == EDF)
		req.key = coder->last_compile_start + sim->config.time_to_burnout;
	else
		req.key = req.seq;
	return (req);
}

void	order_dongles(t_coder *coder, t_dongle **first, t_dongle **second)
{
	if (coder->left->id < coder->right->id)
	{
		*first = coder->left;
		*second = coder->right;
	}
	else
	{
		*first = coder->right;
		*second = coder->left;
	}
}
