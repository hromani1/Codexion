/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 21:12:09 by hromani           #+#    #+#             */
/*   Updated: 2026/11/06 23:03:46 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *heap, int capacity, t_scheduler scheduler_type)
{
	heap->data = (t_request *)malloc(sizeof(t_request) * capacity);
	if (!heap->data)
		return (0);
	heap->capacity = capacity;
	heap->size = 0;
	if (scheduler_type == FIFO)
		heap->cmp = cmp_fifo;
	else if (scheduler_type == EDF)
		heap->cmp = cmp_edf;
	return (1);
}

void	heap_push(t_heap *heap, t_request req)
{
	int	working_index;

	working_index = heap->size;
	heap->data[heap->size] = req;
	heap->size++;
	while (working_index != 0
		&& heap->cmp(&heap->data[working_index],
			&heap->data[(working_index - 1) / 2]) < 0)
	{
		swap(&heap->data[working_index], &heap->data[(working_index - 1) / 2]);
		working_index = (working_index - 1) / 2;
	}
}

t_request	heap_pop(t_heap *heap)
{
	t_request	min;

	min = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];
	heapify_down(heap, 0);
	return (min);
}
