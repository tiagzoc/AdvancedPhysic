#pragma once

#include "framework.h"
#include "Shader.h"

struct ComputeBuffer
{
	GLuint ssbo;
	GLuint binding;
	virtual void			Init() = 0;
	virtual void			Update(void* ptr) = 0;
	virtual const void*		StartRead() = 0;
	virtual void			EndRead() = 0;
	virtual GLenum			GetType() = 0;
};

template<typename T>
struct AtomicBuffer : public ComputeBuffer
{
	T default_value						= 0;
	const uint64_t element_byte_size	= sizeof(T);

	virtual void Init() override
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ssbo);
		glBufferData(
			GL_ATOMIC_COUNTER_BUFFER,
			element_byte_size,
			&default_value,
			GL_DYNAMIC_DRAW
		);
	}
	virtual void Update(void* ptr = nullptr) override
	{
		const T* pSrc = (T*)ptr;
		if (pSrc == nullptr) pSrc = &default_value;

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ssbo);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, element_byte_size, pSrc);
	}
	virtual const void* StartRead() override {
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ssbo);
		return glMapBuffer(
			GL_ATOMIC_COUNTER_BUFFER,
			GL_READ_ONLY
		);
	}
	virtual void EndRead() override {
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	}

	virtual GLenum GetType() override { return GL_ATOMIC_COUNTER_BUFFER;  };
};

template<typename T>
struct ReadBuffer1D : public ComputeBuffer
{
	T* pData							= nullptr;
	uint64_t element_count				= 0;
	const uint64_t element_byte_size	= sizeof(T);

	virtual void Init() override {
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, element_byte_size * element_count, pData, GL_STATIC_DRAW);
	}
	virtual void Update(void* ptr) override {
		Update(ptr, element_count);
	}
	void Update(void* ptr, unsigned int count) {
		pData = (T*)ptr;
		element_count = count; 
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, element_byte_size * count, pData);
	}

	virtual const void* StartRead() override {
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		return glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	}
	virtual void	EndRead() override {
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
	virtual GLenum GetType() override { return GL_SHADER_STORAGE_BUFFER; };
};

template<typename T>
struct WriteBuffer1D : public ComputeBuffer
{
	uint64_t element_count				= 0;
	const uint64_t element_byte_size	= sizeof(T);

	virtual void Init() override {
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, element_byte_size * element_count, nullptr, GL_DYNAMIC_DRAW);
	}
	virtual void Update(void* ptr) override { }
	virtual const void* StartRead() override {
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		return glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	}
	virtual void	EndRead() override {
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
	virtual GLenum GetType() override { return GL_SHADER_STORAGE_BUFFER; };
};

class ComputeShader
{
	ComputeProgram* mpProgram;
	std::vector<ComputeBuffer*> mpBuffers;
	glm::uvec3 mGroupSize;
	glm::uvec3 mElementCount;
public:
	ComputeShader();

	void Init(ComputeProgram* pProgram, glm::uvec3 group_size, glm::uvec3 element_count);

	ComputeProgram* ActivateProgram();
	void SetElementCount(glm::uvec3 element_count);

	template<typename T>
	AtomicBuffer<T>* CreateAtomicBuffer() {
		AtomicBuffer<T>* pBuffer = new AtomicBuffer<T>();
		mpBuffers.push_back(pBuffer);
		return pBuffer;
	}

	template<typename T>
	ReadBuffer1D<T>* CreateReadBuffer()
	{
		ReadBuffer1D<T>* pBuffer = new ReadBuffer1D<T>();
		mpBuffers.push_back(pBuffer);
		return pBuffer;
	}

	template<typename T>
	WriteBuffer1D<T>* CreateWriteBuffer()
	{
		WriteBuffer1D<T>* pBuffer = new WriteBuffer1D<T>();
		mpBuffers.push_back(pBuffer);
		return pBuffer;
	}

	void Compute();

};

