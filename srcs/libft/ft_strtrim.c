/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/04 15:10:42 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/22 19:30:38 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	int	ft_searching_e(char const *s1, char const *s, unsigned int end)
{
	unsigned int	j;

	j = 0;
	while (s[j])
	{
		if (s[j] == s1[end])
		{
			end--;
			j = 0;
		}
		else
			j++;
	}
	return (end);
}

static	int	ft_searching_s(char const *s1, char const *s, unsigned int sta)
{
	unsigned int	j;

	j = 0;
	while (s[j])
	{
		if (s[j] == s1[sta])
		{
			sta++;
			j = 0;
		}
		else
			j++;
	}
	return (sta);
}

char		*ft_strtrim(char const *s1, char const *set)
{
	char			*ptr;
	unsigned int	i;
	unsigned int	start;
	unsigned int	end;

	i = 0;
	start = 0;
	if (!s1 || !set)
		return (NULL);
	end = ft_strlen(s1) - 1;
	while (end >= i && s1[i] != '\0')
	{
		if (i == 0 && end >= start)
		{
			start = ft_searching_s(s1, set, start);
			i = start;
		}
		if (i == end)
			end = ft_searching_e(s1, set, end);
		i++;
	}
	if (!(ptr = ft_substr(s1, start, (end - start) + 1)))
		return (NULL);
	return (ptr);
}
