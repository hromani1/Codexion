/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/07 22:17:26 by hromani           #+#    #+#             */
/*   Updated: 2026/11/11 22:23:37 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	debug_refactor(t_simulation *sim, t_coder *coder)
{
	t_config		*config;

	config = &sim->config;
	log_msg(sim, coder->id, "is debugging");
	precise_sleep(config->time_to_debug, sim);
	log_msg(sim, coder->id, "is refactoring");
	precise_sleep(config->time_to_refactor, sim);
}

void	*coder_routine(void *thread_coder)
{
	t_coder			*coder;
	t_simulation	*sim;
	t_config		*config;

	coder = (t_coder *)thread_coder;
	sim = coder->sim;
	config = &sim->config;
	while (!is_stopped(sim))
	{
		if (!acquire_both(coder, sim))
			break ;
		pthread_mutex_lock(&coder->lock);
		coder->last_compile_start = elapsed_time(sim);
		pthread_mutex_unlock(&coder->lock);
		log_msg(sim, coder->id, "is compiling");
		precise_sleep(config->time_to_compile, sim);
		release_both_dongles(coder, sim);
		pthread_mutex_lock(&coder->lock);
		coder->compile_count += 1;
		pthread_mutex_unlock(&coder->lock);
		debug_refactor(sim, coder);
	}
	return (NULL);
}
