/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 16:48:54 by gchopin           #+#    #+#             */
/*   Updated: 2020/06/09 11:00:56 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	const char		*mem_src;
	char			*mem_dst;

	mem_src = (const char *)src;
	mem_dst = (char *)dst;
	if (dst > src)
	{
		while (len > 0)
		{
			mem_dst[len - 1] = mem_src[len - 1];
			len = len - 1;
		}
	}
	else
	{
		ft_memcpy(mem_dst, mem_src, len);
	}
	return (dst);
}
