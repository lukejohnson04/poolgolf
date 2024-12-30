
struct Camera
{
    v2 pos={0.f,0.f};
    // Zoom is percentage larger everything is
    // E.g. 2 would mean everything is scaled up 2x
    float zoom = 1.f;
};

glm::mat4 CalculateCameraViewMatrix(Camera *camera)
{
    glm::mat4 view = glm::mat4(1.0);
    view = glm::translate(
        view,
        glm::vec3(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 0.0f));

    view = glm::scale(
        view,
        glm::vec3(camera->zoom, camera->zoom, 1.0f));

    view = glm::translate(
        view,
        glm::vec3(-WINDOW_WIDTH * 0.5f, -WINDOW_HEIGHT * 0.5f, 0.0f));
    
    view = glm::translate(
        view,
        glm::vec3(-camera->pos.x, -camera->pos.y, 0.0f));
    
    return view;
}
