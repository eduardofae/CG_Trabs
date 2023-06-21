// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/matrices.hpp"

typedef struct {
	bool w, a, s, d, space, shift;
} PressedKeys;

typedef struct {
	float angleX, angleY;
} Angles;

glm::vec4 moveCam(glm::vec4 view_vec, glm::vec4 up, glm::vec4 camera_pos, PressedKeys g_keys, bool g_reset, glm::vec4 g_cameraInitialPosition);
glm::vec4 rotateCam(glm::vec4 view_vec, glm::vec4 up, Angles *g_angles);