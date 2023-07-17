/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/04 12:55:08 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/05 11:28:36 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_len(char *str)
{
	size_t i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

char			*ft_strjoin(char const *s1, char const *s2)
{
	char			*ptr;
	unsigned int	i;
	unsigned int	j;

	i = 0;
	j = 0;
	if (!s1 || !s2)
		return (NULL);
	if (!(ptr = malloc((sizeof(char const *) * ft_len((char *)s1))
					+ (sizeof(char const *) * ft_len((char *)s2)) + 1)))
		return (NULL);
	while (s1[i] != '\0')
	{
		ptr[i] = (char)s1[i];
		i++;
	}
	while (s2[j] != '\0')
	{
		ptr[i] = (char)s2[j];
		i++;
		j++;
	}
	ptr[i] = '\0';
	return (ptr);
}
