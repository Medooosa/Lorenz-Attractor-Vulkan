#pragma once

#include "device.h"
#include "uniform_buffer.h"
#include "utils.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


#include <vector>
#include <cassert>
#include <memory>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace Lorenz 
{
	class Model 
	{
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescription();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();

			bool operator==(const Vertex& other) const 
			{
				return position == other.position && color == other.color && normal == other.normal &&
					uv == other.uv;
			}

		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		Model(LorenzDevice &device, const Model::Builder& builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static std::unique_ptr<Model> createModelFromFile(LorenzDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>& indices);

		LorenzDevice& lorenzDevice;

		std::unique_ptr<LorenzBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false; 

		std::unique_ptr<LorenzBuffer> indexBuffer;
		uint32_t indexCount;
	};

}	// namespace Lorenz