/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 19:44:53 by hromani           #+#    #+#             */
/*   Updated: 2026/11/06 23:04:42 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <limits.h>
# include <string.h>
# include <sys/time.h>

typedef struct s_coder		t_coder;
typedef struct s_request	t_request;
typedef struct s_heap		t_heap;
typedef struct s_dongle		t_dongle;
typedef struct s_config		t_config;
typedef struct s_simulation	t_simulation;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	long			last_compile_start;
	int				compile_count;
	pthread_mutex_t	lock;
	t_dongle		*left;
	t_dongle		*right;
	t_simulation	*sim;
}	t_coder;

typedef struct s_request
{
	int		coder_id;
	long	key;
	long	seq;
}	t_request;

typedef struct s_heap
{
	t_request	*data;
	int			capacity;
	int			size;
	int			(*cmp)(t_request *a, t_request *b);
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	int				in_use;
	long			available_at;
	pthread_cond_t	cond;
	t_heap			queue;
	int				id;
}	t_dongle;

typedef struct s_config
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_simulation
{
	t_config		config;
	long			start_time;
	int				stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor;
}	t_simulation;

int			parse(int argc, char **argv, t_config *config);
int			is_entirely_int(char *ptr);
long		cmp_fifo(t_request *a, t_request *b);
long		cmp_edf(t_request *a, t_request *b);
void		swap(t_request *a, t_request *b);
void		heapify_down(t_heap *heap, int i);
int			error(char *msg);
long long	get_timestamp_ms(void);

#endif