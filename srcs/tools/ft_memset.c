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