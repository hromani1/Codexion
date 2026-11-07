/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 21:22:09 by hromani           #+#    #+#             */
/*   Updated: 2026/11/06 23:04:15 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	cmp_fifo(t_request *a, t_request *b)
{
	return (a->key - b->key);
}

long	cmp_edf(t_request *a, t_request *b)
{
	if (a->key != b->key)
		return (a->key - b->key);
	return (a->seq - b->seq);
}

void	swap(t_request *a, t_request *b)
{
	t_request	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	heapify_down(t_heap *heap, int i)
{
	int	smallest;
	int	left;
	int	right;

	smallest = i;
	left = 2 * i + 1;
	right = 2 * i + 2;
	if (left < heap->size
		&& heap->cmp(&heap->data[left], &heap->data[smallest]) < 0)
		smallest = left;
	if (right < heap->size
		&& heap->cmp(&heap->data[right], &heap->data[smallest]) < 0)
		smallest = right;
	if (smallest != i)
	{
		swap(&heap->data[i], &heap->data[smallest]);
		heapify_down(heap, smallest);
	}
}
