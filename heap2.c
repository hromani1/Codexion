/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 21:30:10 by hromani           #+#    #+#             */
/*   Updated: 2026/11/06 23:02:55 by hromani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	heap_peek(t_heap *heap)
{
	return (heap->data[0]);
}

int	heap_is_empty(t_heap *heap)
{
	if (heap->size == 0)
		return (1);
	return (0);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->data);
	heap->data = NULL;
	heap->size = 0;
	heap->capacity = 0;
}
