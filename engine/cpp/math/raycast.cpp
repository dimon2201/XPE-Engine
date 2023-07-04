namespace xpe {

    namespace math {

        Ray RayCast::ToLocalSpace(const Ray &ray, const glm::mat4 &modelMatrix) {
            return { glm::inverse(modelMatrix) * ray.Origin };
        }

        Ray RayCast::ToWorldSpace(const Ray &ray, const glm::mat4 &modelMatrix) {
            return { modelMatrix * ray.Origin };
        }

        Ray RayCast::ToWorldSpaceInverse(const Ray &ray, const glm::mat4 &viewMatrix) {
            return { glm::inverse(viewMatrix) * ray.Origin };
        }

        Ray RayCast::ToViewSpace(const Ray &ray, const glm::mat4 &viewMatrix) {
            return { viewMatrix * ray.Origin };
        }

        Ray RayCast::ToViewSpaceInverse(const Ray &ray, const float fov, const float aspectRatio) {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x / focalLength, ray.Origin.y * aspectRatio / focalLength, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        Ray RayCast::ToViewSpaceInverse(const Ray &ray, const glm::mat4 &perspectiveMatrix) {
            return { glm::inverse(perspectiveMatrix) * ray.Origin };
        }

        Ray RayCast::ToClipSpace(const Ray &ray, const float fov, const float aspectRatio) {
            float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
            glm::vec4 newOrigin = { ray.Origin.x * focalLength, ray.Origin.y * focalLength / aspectRatio, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        Ray RayCast::ToClipSpace(const Ray &ray, const glm::mat4 &perspectiveMatrix) {
            return {perspectiveMatrix * ray.Origin };
        }

        Ray RayCast::ToClipSpaceInverse(const Ray &ray) {
            glm::vec4 newOrigin = { ray.Origin.x * ray.Origin.w, ray.Origin.y * ray.Origin.w, ray.Origin.z * ray.Origin.w, ray.Origin.w };
            return { newOrigin };
        }

        Ray RayCast::ToNDCSpace(const Ray &ray) {
            glm::vec4 newOrigin = { ray.Origin.x / ray.Origin.w, ray.Origin.y / ray.Origin.w, ray.Origin.z / ray.Origin.w, ray.Origin.w / ray.Origin.w };
            return { newOrigin };
        }

        Ray RayCast::ToNDCSpaceInverse(const Ray &ray, int width, int height) {
            glm::vec4 newOrigin = { (ray.Origin.x * 2 / (float) width) - 1, 1 - (ray.Origin.y * 2 / (float) height), 1.0f, ray.Origin.w };
            return { newOrigin };
        }

        Ray RayCast::ToScreenSpace(const Ray &ray, int width, int height) {
            glm::vec4 newOrigin = { (ray.Origin.x + 1) * (float) width / 2, (ray.Origin.y + 1) * (float) height / 2, 1.0f, ray.Origin.w };
            return { newOrigin };
        }

    }

}