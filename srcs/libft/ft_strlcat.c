/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 16:48:14 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/07 17:49:43 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	j;
	size_t	k;

	i = 0;
	j = 0;
	k = 0;
	while (src[k])
		k++;
	while (dst[i] != '\0' && dstsize > i)
		i++;
	if (dstsize == 0)
		return (k);
	if (i >= dstsize)
		return (k + dstsize);
	while (((dstsize - i) - 1 > j) && src[j] != '\0')
	{
		dst[i + j] = src[j];
		j++;
	}
	dst[j + i] = '\0';
	return (i + k);
}
