/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: nhariman <nhariman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/27 14:43:07 by nhariman      #+#    #+#                 */
/*   Updated: 2022/07/14 21:11:02 by salbregh      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#include "src/config/nginx_config.hpp"
#include "src/config/server_block.hpp"

int main(int ac, const char **av) {
	(void)ac;
	try {
		NginxConfig		input_file(av[1]);
		// std::cout << "Amount of serverblocks: " << input_file.GetServerBlockAmount() << std::endl;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
