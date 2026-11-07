/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hromani <hromani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 22:23:13 by hromani           #+#    #+#             */
/*   Updated: 2026/11/06 23:02:46 by hromani          ###   ########.fr       */
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
