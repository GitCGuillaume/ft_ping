/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/04 12:08:12 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/07 19:36:21 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*ptr;
	size_t	i;

	i = 0;
	if (!s)
		return (NULL);
	if (!(ptr = malloc((sizeof(char) * len) + 1)))
		return (NULL);
	if (ft_strlen(s) < start)
	{
		ptr[i] = '\0';
		return (ptr);
	}
	while (len > i && s[start] != '\0')
	{
		ptr[i] = (char)s[start];
		start++;
		i++;
	}
	ptr[i] = '\0';
	return (ptr);
}
