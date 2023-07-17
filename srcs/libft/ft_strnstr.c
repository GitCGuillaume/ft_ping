/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 17:04:26 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/08 17:36:31 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *string, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (needle[0] == '\0')
		return ((char *)string);
	while (string[i] != '\0' && len > i)
	{
		j = 0;
		if (string[i] == needle[j])
		{
			while (needle[j] == string[i + j]
					&& needle[j] != '\0' && len > i + j)
			{
				if (needle[j + 1] == '\0')
					return ((char *)&string[i]);
				j++;
			}
		}
		i++;
	}
	return (0);
}
