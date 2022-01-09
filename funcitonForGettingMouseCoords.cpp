void ScreenPosToWorldRay(
	GLFWwindow* window,
	glm::mat4 ViewMatrix,              
	glm::mat4 ProjectionMatrix,
	glm::mat4 ModelMatrix,
	glm::vec3& out_origin,             
	glm::vec3& out_direction	         
) {
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	mouseX += 0.5;
	mouseY = height - 0.5 - mouseY;

	int viewPortDimensions[4];
	glGetIntegerv(GL_VIEWPORT, viewPortDimensions);

	glm::vec4 lRayStart_NDC(
		(((float)mouseX - (float)viewPortDimensions[0]) / (float)viewPortDimensions[2] - 0.5f) * 2.0f,
		(((float)mouseY - (float)viewPortDimensions[1]) / (float)viewPortDimensions[3] - 0.5f) * 2.0f,
		-1.0,
		1.0f
	);
	glm::vec4 lRayEnd_NDC(
		(((float)mouseX-(float)viewPortDimensions[0]) / (float)viewPortDimensions[2] - 0.5f) * 2.0f,
		(((float)mouseY - (float)viewPortDimensions[1]) / (float)viewPortDimensions[3] - 0.5f) * 2.0f, 1.0, 1.0f);

	glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix * ModelMatrix);
	glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
	glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;


	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);

	//out_origin = glm::vec3(camera.Position);
	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
