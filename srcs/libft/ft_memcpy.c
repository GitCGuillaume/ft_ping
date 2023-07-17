/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/28 15:06:06 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/07 11:58:27 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	const char		*mem_src;
	unsigned char	*mem_dst;
	size_t			i;

	mem_src = src;
	mem_dst = dst;
	i = 0;
	if (dst == NULL && src == NULL)
		return (NULL);
	while (n > 0)
	{
		mem_dst[i] = mem_src[i];
		i++;
		n--;
	}
	return ((void *)dst);
}
