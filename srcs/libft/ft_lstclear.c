/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/11 13:45:24 by gchopin           #+#    #+#             */
/*   Updated: 2020/05/13 11:06:24 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list *keep_addresses;

	if (!lst || !*lst)
		return ;
	if (*lst && del)
	{
		while (*lst != NULL)
		{
			keep_addresses = (*lst)->next;
			ft_lstdelone(*lst, del);
			*lst = keep_addresses;
		}
		*lst = NULL;
	}
}
