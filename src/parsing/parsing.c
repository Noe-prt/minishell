/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nopareti <nopareti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 04:39:18 by nopareti          #+#    #+#             */
/*   Updated: 2025/01/14 04:39:18 by nopareti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	parse_env_vars(char **args, t_env *envp)
{
	int		i;
	int		j;
	int		k;
	int		in_quotes;
	char	*var_name;
	char	*var_value;
	char	*new_arg;

	i = 0;
	while (args[i])
	{
		j = 0;
		in_quotes = 0;
		while (args[i][j])
		{
			if (args[i][j] == '\'')
				in_quotes = !in_quotes;
			if (!in_quotes && args[i][j] == '$' && args[i][j + 1])
			{
				if (!is_valid_var_char(args[i][j + 1], 1))
				{
					j++;
					continue ;
				}
				k = j + 1;
				while (args[i][k] && is_valid_var_char(args[i][k], 0))
					k++;
				var_name = ft_substr(args[i], j + 1, k - (j + 1));
				var_value = ft_getenv(var_name, envp);
				new_arg = replace_env_var(args[i], j, var_value, k - j - 1);
				if (new_arg)
				{
					free(args[i]);
					args[i] = new_arg;
					j--;
				}
				free(var_name);
			}
			j++;
		}
		i++;
	}
}

static char	*remove_quotes_from_str(char *str)
{
	int		i;
	int		j;
	int		len;
	char	*new_str;
	int		in_single_quote;
	int		in_double_quote;

	len = ft_strlen(str);
	new_str = malloc(sizeof(char) * (len + 1));
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (str[i] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		else
			new_str[j++] = str[i];
		i++;
	}
	new_str[j] = '\0';
	return (new_str);
}

char	**remove_out_quotes(char **args)
{
	int		i;
	int		len;
	char	**new_args;
	char	*tmp;

	if (!args)
		return (NULL);
	len = 0;
	while (args[len])
		len++;
	new_args = malloc(sizeof(char *) * (len + 1));
	if (!new_args)
		return (NULL);
	i = 0;
	while (args[i])
	{
		tmp = remove_quotes_from_str(args[i]);
		if (!tmp)
		{
			free_strs(new_args);
			return (NULL);
		}
		new_args[i] = tmp;
		i++;
	}
	new_args[i] = NULL;
	return (new_args);
}

int	count_redirections(char *cmd_line)
{
	int	i;
	int	nb_redirections;
	int	in_single_quote;
	int	in_double_quote;

	i = 0;
	nb_redirections = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (cmd_line[i])
	{
		if (cmd_line[i] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		else if (cmd_line[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		if (!in_single_quote && !in_double_quote)
		{
			if (cmd_line[i] == '<' || cmd_line[i] == '>')
			{
				nb_redirections++;
				if (cmd_line[i + 1] && cmd_line[i] == cmd_line[i + 1])
					i++;
			}
		}
		i++;
	}
	return (nb_redirections);
}

char	*get_redir_file(char *cmd_line, int i)
{
	char	*redir_file;
	int	file_len;
	int	j;

	file_len = 0;
	j = 0;
	while (cmd_line[i] && (cmd_line[i] == 32 || cmd_line[i] == '\t'))
		i++;
	while (cmd_line[file_len] != 32 && cmd_line[file_len] != '\t')
		file_len++;
	redir_file = malloc(sizeof(char) * (file_len + 1));
	if (!redir_file)
		return (NULL);
	while (cmd_line[i] && cmd_line[i] != 32 && cmd_line[i] != '\t')
	{
		redir_file[j] = cmd_line[i];
		i++;
		j++;
	}
	redir_file[j] = '\0';
	return (redir_file);
}

void	parse_redirections(t_cmd *cmd, char *cmd_line)
{
	int		i;
	int		redir_index;
	int		in_single_quote;
	int		in_double_quote;

	i = 0;
	redir_index = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	cmd->nb_redirections = count_redirections(cmd_line);
	cmd->redirections = malloc(sizeof(t_redirection) * cmd->nb_redirections);
	if (!cmd->redirections)
		return ;
	while (cmd_line[i])
	{
		if (cmd_line[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (cmd_line[i] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		if (!in_single_quote && !in_double_quote)
		{
			if (cmd_line[i] == '>')
			{
				if (cmd_line[i + 1] == '>')
				{
					i++;
					cmd->redirections[redir_index].type = 2; // >>
				}
				else
				{
					cmd->redirections[redir_index].type = 1; // >
				}
				cmd->redirections[redir_index].file = remove_quotes_from_str(get_redir_file(cmd_line, ++i));
				redir_index++;
			}
			else if (cmd_line[i] == '<')
			{
				if (cmd_line[i + 1] == '<')
				{
					i++;
					cmd->redirections[redir_index].type = 4; // <<
				}
				else
				{
					cmd->redirections[redir_index].type = 3; // <
				}
				cmd->redirections[redir_index].file = remove_quotes_from_str(get_redir_file(cmd_line, ++i));
				redir_index++;
			}
		}
		i++;
	}
}

t_cmd	parse_cmd(char *cmd_line, t_env *envp, int pipe_presence)
{
	//t_redirection	*redirections;
	char	**args;
	t_cmd	cmd;
	//int	i;

	//i = 0;
	cmd.nb_redirections = count_redirections(cmd_line);
	if (cmd.nb_redirections > 0)
		parse_redirections(&cmd, cmd_line);
	args = ft_split_args(remove_out_spaces(cmd_line));
	parse_env_vars(args, envp);
	if (!args)
	{
		printf("minishell: %s not found!\n", cmd_line);
		cmd.args = NULL;
		return (cmd);
	}
	cmd.args = remove_out_quotes(args);
	if (!cmd.args)
	{
		printf("minishell: %s not found!\n", cmd_line);
		cmd.args = NULL;
		return (cmd);
	}
	cmd.pipe_presence = pipe_presence;
	return (cmd);
}

int	is_only_spaces(char *str)
{
	while (*str)
	{
		if (*str != ' ' && *str != '\t')
			return (0);
		str++;
	}
	return (1);
}

t_cmd_line	parse_cmd_line(char *line, t_env *envp)
{
	int	i;
	t_cmd_line	cmd_line;
	char	**splitted_cmds;

	i = 0;
	cmd_line.cmds = NULL;
	cmd_line.nb_cmds = 0;
	if (!line || !*line)
		return (cmd_line);
	if (is_only_spaces(line))
		return (cmd_line);
	splitted_cmds = ft_split(line, '|');
	if (!splitted_cmds)
	{
		printf("minishell: %s not found", line);
		return (cmd_line);
	}
	cmd_line.nb_cmds = 0;
	while (splitted_cmds[cmd_line.nb_cmds])
		cmd_line.nb_cmds++;
	cmd_line.cmds = malloc(sizeof(t_cmd) * (cmd_line.nb_cmds + 1));
	while (splitted_cmds[i])
	{
		if (i < cmd_line.nb_cmds - 1)
			cmd_line.cmds[i] = parse_cmd(splitted_cmds[i], envp, 1);
		else
			cmd_line.cmds[i] = parse_cmd(splitted_cmds[i], envp, 0);
		
		i++;
	}
	return (cmd_line);
}