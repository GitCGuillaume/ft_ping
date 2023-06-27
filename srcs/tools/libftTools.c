#include <tools.h>

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*update_char;
	size_t			i;

	update_char = b;
	i = 0;
	while (len > i)
	{
		update_char[i] = (unsigned char)c;
		i++;
	}
	return ((void *)b);
}

size_t ft_strlen(const char *str) {
    size_t i = 0;

    while (str[i])
        ++i;
    return (i);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	const char		*mem_src;
	unsigned char	*mem_dst;
	size_t			i;

	mem_src = src;
	mem_dst = dst;
	i = 0;
	if (dst == NULL && src == NULL)
		return (NULL);
	while (n > 0)
	{
		mem_dst[i] = mem_src[i];
		i++;
		n--;
	}
	return ((void *)dst);
}