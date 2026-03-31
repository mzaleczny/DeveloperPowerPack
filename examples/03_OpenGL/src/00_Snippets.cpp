#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

	glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z));
	M = glm::rotate(M, Time, glm::vec3(0.0f, 0.0f, 1.0f));
	M = glm::translate(M, -glm::vec3(v.x, v.y, v.z));
	std::cout << std::setw(10) << std::setprecision(4) << M[0][0]
		<< std::setw(10) << std::setprecision(4) << M[1][0]
		<< std::setw(10) << std::setprecision(4) << M[2][0]
		<< std::setw(10) << std::setprecision(4) << M[3][0] << std::endl;

	std::cout << std::setw(10) << std::setprecision(4) << M[0][1]
		<< std::setw(10) << std::setprecision(4) << M[1][1]
		<< std::setw(10) << std::setprecision(4) << M[2][1]
		<< std::setw(10) << std::setprecision(4) << M[3][1] << std::endl;

	std::cout << std::setw(10) << std::setprecision(4) << M[0][2]
		<< std::setw(10) << std::setprecision(4) << M[1][2]
		<< std::setw(10) << std::setprecision(4) << M[2][2]
		<< std::setw(10) << std::setprecision(4) << M[3][2] << std::endl;

	std::cout << std::setw(10) << std::setprecision(4) << M[0][3]
		<< std::setw(10) << std::setprecision(4) << M[1][3]
		<< std::setw(10) << std::setprecision(4) << M[2][3]
		<< std::setw(10) << std::setprecision(4) << M[3][3] << std::endl;
	return SDL_APP_SUCCESS;
