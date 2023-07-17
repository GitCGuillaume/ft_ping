/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 17:07:32 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/07 12:07:15 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int		ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*mem_s1;
	const unsigned char	*mem_s2;
	size_t				i;

	i = 0;
	mem_s1 = s1;
	mem_s2 = s2;
	while (n > i)
	{
		if (mem_s1[i] - mem_s2[i] != 0)
			return (mem_s1[i] - mem_s2[i]);
		i++;
	}
	return (0);
}
