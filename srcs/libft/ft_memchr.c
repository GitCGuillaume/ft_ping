/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 16:41:51 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/07 12:05:31 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*mem_string;
	size_t			i;

	mem_string = (unsigned char *)s;
	i = 0;
	while (n > i)
	{
		if ((unsigned char)c == (unsigned char)mem_string[i])
		{
			return (&mem_string[i]);
		}
		i++;
	}
	return (NULL);
}
