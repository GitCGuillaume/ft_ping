/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/09 12:37:25 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/22 18:08:42 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **alst, t_list *new)
{
	t_list *t_lst;

	if (new)
	{
		if (*alst && alst)
		{
			t_lst = ft_lstlast(*alst);
			if (t_lst)
			{
				t_lst->next = new;
			}
		}
		else
			*alst = new;
	}
}
