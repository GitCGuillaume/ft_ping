/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 17:02:47 by gchopin           #+#    #+#             */
/*   Updated: 2020/06/09 15:00:30 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	char	*located_c;
	int		i;

	located_c = (char *)s;
	i = 0;
	while (s[i] != '\0')
	{
		if (located_c[i] == c)
		{
			return (&located_c[i]);
		}
		i++;
	}
	if (located_c[i] == c)
		return (&located_c[i]);
	return (0);
}
