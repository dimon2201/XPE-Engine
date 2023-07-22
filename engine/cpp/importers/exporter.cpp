#include <importers/exporter.h>

#define CGLTF_WRITE_IMPLEMENTATION
#include <cgltf_write.h>

namespace xpe {

    namespace io {

        bool Exporter::Export(const char* filepath, const math::Model3D &model) {
            cgltf_options options = {};
            cgltf_data data = {};

            FromGLTFModel(model, &data);

            cgltf_result result = cgltf_write_file(&options, filepath, &data);

            if (result != cgltf_result_success) {
                LogInfo("Exported GLTF model into file {}", filepath);
                return true;
            }

            else {
                LogError("Failed to export GLTF into file {}", filepath);
                return false;
            }
        }

        void Exporter::FromGLTFModel(const math::Model3D &model, void* data) {

        }

    }

}