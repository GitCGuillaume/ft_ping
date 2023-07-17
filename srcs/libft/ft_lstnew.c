/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchopin <gchopin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/08 14:08:51 by gchopin           #+#    #+#             */
/*   Updated: 2020/06/19 11:35:47 by gchopin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*t_lst;

	if (!(t_lst = malloc(sizeof(t_list))))
		return (NULL);
	t_lst->content = content;
	t_lst->next = NULL;
	return (t_lst);
}
