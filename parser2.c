#include "codexion.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int	is_entirely_int(char *ptr)
{
	int	i;

	i = 0;
	if (ptr == NULL || *ptr == '\0')
		return (0);
	while (ptr[i] == '\n' || ptr[i] == ' ' || ptr[i] == '\f' || ptr[i] == '\r'
		|| ptr[i] == '\t' || ptr[i] == '\v')
		i++;
	while (ptr[i] != '\0')
	{
		if (!ft_isdigit(ptr[i]))
			return (0);
		i++;
	}
	return (1);
}
