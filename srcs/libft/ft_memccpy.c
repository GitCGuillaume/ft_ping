/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/28 16:56:01 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/13 20:41:46 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memccpy(void *dst, const void *src, int c, size_t n)
{
	const	unsigned	char	*mem_src;
	unsigned	char			*mem_dst;
	size_t						i;

	mem_src = src;
	mem_dst = dst;
	i = 0;
	while (n > 0)
	{
		mem_dst[i] = mem_src[i];
		if ((unsigned char)c == mem_src[i])
		{
			return (&mem_dst[i + 1]);
		}
		n--;
		i++;
	}
	return (NULL);
}
