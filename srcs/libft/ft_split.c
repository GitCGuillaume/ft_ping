/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/05 18:25:11 by gchopin           #+#    #+#             */
/*   Updated: 2020/06/23 16:23:49 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t		count_word(char const *s, char c)
{
	size_t	i;
	size_t	nb_word;

	i = 0;
	nb_word = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c)
		{
			nb_word++;
			while (s[i] != '\0' && s[i] != c)
			{
				i++;
			}
		}
		else
			i++;
	}
	return (nb_word);
}

static void			*ft_free(char **ptr, size_t size)
{
	size++;
	while (size != 0)
	{
		size--;
		free(ptr[size]);
	}
	free(ptr);
	return (NULL);
}

static	char		*crea_arr(char const *s, char c, char *ptr, size_t *i)
{
	unsigned int	start;
	size_t			end;

	start = 0;
	end = 0;
	while (s[*i] == c && s[*i] != '\0')
		*i = *i + 1;
	if (s[*i] != c && s[*i] != '\0')
	{
		start = *i;
		*i = *i + 1;
	}
	while (s[*i] != c && s[*i] != '\0')
		*i = *i + 1;
	if (s[*i] == c || s[*i] == '\0')
	{
		end = *i;
		*i = *i + 1;
	}
	return (ptr = ft_substr(s, start, (end - start)));
}

char				**ft_split(char const *s, char c)
{
	char			**ptr;
	size_t			nb_word;
	size_t			i;
	size_t			j;

	i = 0;
	j = 0;
	if (!s)
		return (NULL);
	nb_word = count_word(s, c);
	if (!(ptr = malloc(sizeof(char const *) * (nb_word + 1))))
		return (NULL);
	while (s[i] != '\0' && s[i] == c)
		i++;
	while (nb_word > j)
	{
		if (!(ptr[j] = crea_arr(s, c, ptr[j], &i)))
			return (ft_free(ptr, j));
		j++;
	}
	ptr[j] = 0;
	return (ptr);
}
