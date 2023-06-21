#include "camera.hpp"

glm::vec4 moveCam(glm::vec4 view_vec, glm::vec4 up, glm::vec4 camera_pos, PressedKeys g_keys, bool g_reset, glm::vec4 g_cameraInitialPosition, float delta_time){
    float delta = 500.0f;
    glm::vec4 side_vec = crossproduct(view_vec, up);
    glm::vec4 up_vec = crossproduct(side_vec, view_vec);
    if(g_keys.w)
        camera_pos += delta * delta_time * view_vec;
    if(g_keys.s)
        camera_pos -= delta * delta_time * view_vec;
    if(g_keys.d)
        camera_pos += delta * delta_time * side_vec;
    if(g_keys.a)
        camera_pos -= delta * delta_time * side_vec;
    if(g_keys.space)
        camera_pos += delta * delta_time * up_vec;
    if(g_keys.shift)
        camera_pos -= delta * delta_time * up_vec;
    if(g_reset)
        camera_pos = g_cameraInitialPosition;
    return camera_pos;
}

glm::vec4 rotateCam(glm::vec4 view_vec, glm::vec4 up, Angles *g_angles){
    if(g_angles->angleX == 0 && g_angles->angleY == 0) return view_vec;

    float cam_look_speed = 0.3f;

    glm::vec4 side_vec = crossproduct(view_vec, up);
    glm::vec4 up_vec = crossproduct(side_vec, view_vec);

	// ROTAÇÃO VERTICAL
    glm::vec4 lado = crossproduct(up_vec, view_vec); // Calcula o lado, para rotacionar verticalmente
    glm::vec4 aux = view_vec * Matrix_Rotate(-g_angles->angleY * cam_look_speed, lado);   // Rotação no eixo lado (vertical)

    // TRAVA DA ROTAÇÃO VERTICAL
    if(dotproduct(lado, crossproduct(up, aux)) > 0) { // Testa se o novo valor de lado é igual ao antigo
        view_vec = aux;                                   // Caso seja, salva o novo camera_view (permite a rotação)
	}
    // ROTAÇÃO HORIZONTAL
    view_vec = view_vec * Matrix_Rotate(g_angles->angleX * cam_look_speed, up_vec); // Rotação no eixo up (horizontal)

	g_angles->angleX = 0.0f;
	g_angles->angleY = 0.0f;

    return view_vec;
}