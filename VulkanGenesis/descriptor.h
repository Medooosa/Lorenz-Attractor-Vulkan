#pragma once

#include "device.h"

// std
#include <cassert>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Lorenz {

    class LorenzDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(LorenzDevice& lorenzDevice) : lorenzDevice{ lorenzDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<LorenzDescriptorSetLayout> build() const;

        private:
            LorenzDevice& lorenzDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        LorenzDescriptorSetLayout(
            LorenzDevice& lorenzDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~LorenzDescriptorSetLayout();
        LorenzDescriptorSetLayout(const LorenzDescriptorSetLayout&) = delete;
        LorenzDescriptorSetLayout& operator=(const LorenzDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        LorenzDevice& lorenzDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class LorenzDescriptorWriter;
    };

    class LorenzDescriptorPool {
    public:
        class Builder {
        public:
            Builder(LorenzDevice& lorenzDevice) : lorenzDevice{ lorenzDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<LorenzDescriptorPool> build() const;

        private:
            LorenzDevice& lorenzDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        LorenzDescriptorPool(
            LorenzDevice& LorenzDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~LorenzDescriptorPool();
        LorenzDescriptorPool(const LorenzDescriptorPool&) = delete;
        LorenzDescriptorPool& operator=(const LorenzDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        LorenzDevice& lorenzDevice;
        VkDescriptorPool descriptorPool;

        friend class LorenzDescriptorWriter;
    };

    class LorenzDescriptorWriter {
    public:
        LorenzDescriptorWriter(LorenzDescriptorSetLayout& setLayout, LorenzDescriptorPool& pool);

        LorenzDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        LorenzDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        LorenzDescriptorSetLayout& setLayout;
        LorenzDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace Lorenz