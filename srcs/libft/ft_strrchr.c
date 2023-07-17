/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/02 16:51:25 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/07 12:15:03 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*mem_character;
	int		i;

	mem_character = (char *)s;
	i = 0;
	while (s[i])
		i++;
	while (i != 0)
	{
		if (mem_character[i] == c)
			return (&mem_character[i]);
		i--;
	}
	if (s[i] == c)
		return (&mem_character[i]);
	return (0);
}
