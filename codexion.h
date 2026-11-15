/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 19:44:53 by hromani           #+#    #+#             */
/*   Updated: 2026/11/14 18:45:49 by hromani          ###   ########.fr       */
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
	int				seq_counter;
	pthread_mutex_t	seq_mutex;
	int				globals_ready;
	int				coders_ready;
	int				dongles_ready;
}	t_simulation;

int			is_int_in_range(const char *str);
void		*monitor_routine(void *arg);
int			acquire_both(t_coder *coder, t_simulation *sim);
void		release_both_dongles(t_coder *coder, t_simulation *sim);
void		heap_push(t_heap *heap, t_request req);
int			wait_turn(t_coder *coder, t_dongle *first,
				t_dongle *second, t_simulation *sim);
int			heap_is_empty(t_heap *heap);
void		heap_destroy(t_heap *heap);
t_request	heap_pop(t_heap *heap);
t_request	heap_peek(t_heap *heap);
void		*coder_routine(void *thread_coder);
int			parse(int argc, char **argv, t_config *config);
int			is_entirely_int(char *ptr);
void		heap_destroy(t_heap *heap);
int			cmp_fifo(t_request *a, t_request *b);
int			cmp_edf(t_request *a, t_request *b);
void		swap(t_request *a, t_request *b);
void		heapify_down(t_heap *heap, int i);
int			error(char *msg);
long long	get_timestamp_ms(void);
long long	log_msg(t_simulation *sim, int id, char *msg);
void		precise_sleep(long ms, t_simulation *sim);
int			is_stopped(t_simulation *sim);
long long	elapsed_time(t_simulation *sim);
int			init_simulation(t_simulation *sim, t_config *config);
void		set_stopped(t_simulation *sim);
int			heap_init(t_heap *heap, int capacity, t_scheduler scheduler_type);
int			clean_simulation(t_simulation *sim);
void		heapify_up(t_heap *heap, int i);
int			get_seq(t_simulation *sim);
void		order_dongles(t_coder *coder, t_dongle **first, t_dongle **second);
void		heap_remove(t_heap *heap, int coder_id);
void		release_both_dongles(t_coder *coder, t_simulation *sim);
void		release_dongle(t_dongle *dongle, t_simulation *sim);
t_request	make_request(t_coder *coder, t_simulation *sim);

#endif