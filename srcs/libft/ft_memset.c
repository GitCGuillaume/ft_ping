/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/28 15:02:54 by gchopin           #+#    #+#             */
/*   Updated: 2020/04/30 13:32:03 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*update_char;
	size_t			i;

	update_char = b;
	i = 0;
	while (len > i)
	{
		update_char[i] = (unsigned char)c;
		i++;
	}
	return ((void *)b);
}
