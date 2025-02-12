/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mazeghou <mazeghou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 02:30:01 by mazeghou          #+#    #+#             */
/*   Updated: 2025/01/20 14:05:22 by mazeghou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_printf.h"

int	ft_put_ptr(void *ptr, int first)
{
	unsigned long	num;
	char			*hex;
	int				print_len;

	print_len = 0;
	if (!ptr)
		return (printf_putstr_fd("(nil)", 1));
	if (first == 0)
		print_len += printf_putstr_fd("0x", 1);
	first++;
	num = (unsigned long)ptr;
	hex = "0123456789abcdef";
	if (num >= 16)
		print_len += ft_put_ptr((void *)(num / 16), first);
	printf_putchar_fd(hex[num % 16], 1);
	return (print_len + 1);
}
