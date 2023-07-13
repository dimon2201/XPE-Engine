#include <importers/importer.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

namespace xpe {

    namespace io {

        using namespace core;

        static void ImportAttributes(cgltf_attribute* attributes, usize attributeCount, math::Mesh& mesh) {
            LogInfo("Importing GLTF attributes...");

            for (u32 i = 0 ; i < attributeCount ; i++) {
                auto& attribute = attributes[i];
                LogInfo("Importing GLTF attribute {}", attribute.name);

                u32 index = attribute.index;
                render::Vertex3D vertex;

                switch (attribute.type) {

                    case cgltf_attribute_type_position:
                        break;

                    case cgltf_attribute_type_texcoord:
                        break;

                    case cgltf_attribute_type_normal:
                        break;

                    case cgltf_attribute_type_tangent:
                        break;

                }

                mesh.Vertices.Data.emplace_back(vertex);
                mesh.Indices.Data.emplace_back(index);
            }
        }

        static void ImportMesh(cgltf_mesh* gltfMesh, math::Mesh& mesh) {
            LogInfo("Importing GLTF mesh {}", gltfMesh->name);
            for (u32 i = 0 ; i < gltfMesh->primitives_count ; i++) {
                auto& primitive = gltfMesh->primitives[i];
                LogInfo("Importing GLTF primitives...");
                ImportAttributes(primitive.attributes, primitive.attributes_count, mesh);
            }
        }

        static void ImportScene(cgltf_scene* scene, math::Model3D &model) {
            LogInfo("Importing GLTF scene {}", scene->name);
            for (u32 i = 0 ; i < scene->nodes_count ; i++) {
                auto& node = scene->nodes[i];
                LogInfo("Importing GLTF node {}", node->name);

                math::Mesh mesh;
                ImportMesh(node->mesh, mesh);
                model.Meshes.emplace_back(mesh);

                for (u32 j = 0 ; j < node->children_count ; j++) {
                    auto& child = node->children[j];
                    LogInfo("Importing GLTF children {}", child->name);
                    math::Mesh childMesh;
                    ImportMesh(child->mesh, childMesh);
                    model.Meshes.emplace_back(childMesh);
                }
            }
        }

        static void ImportModel(cgltf_data* root, math::Model3D &model) {
            for (u32 i = 0 ; i < root->scenes_count ; i++) {
                auto& scene = root->scenes[i];
                ImportScene(&scene, model);
            }
        }

        bool Importer::Import(const char* filepath, math::Model3D& model) {
            cgltf_options options;
            memset(&options, 0, sizeof(cgltf_options));
            cgltf_data* data = nullptr;

            cgltf_result result = cgltf_parse_file(&options, filepath, &data);

            if (result == cgltf_result_success) {
                LogInfo("GLTF model parsed from a file {}", filepath);

                result = cgltf_load_buffers(&options, data, filepath);

                if (result == cgltf_result_success) {
                    result = cgltf_validate(data);

                    if (result == cgltf_result_success) {
                        ImportModel(data, model);
                    }

                    else {
                        LogInfo("Failed to validate parsed GLTF data from file {}", filepath);
                        cgltf_free(data);
                        return false;
                    }
                }

                else {
                    LogInfo("Failed to load GLTF buffer from parse data file {}", filepath);
                    cgltf_free(data);
                    return false;
                }

                cgltf_free(data);
                return true;
            }

            else {
                LogError("Failed to parse GLTF from file {}", filepath);
                return false;
            }
        }

    }

}