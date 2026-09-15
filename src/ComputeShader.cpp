#include "ComputeShader.h"
#include "ComputeShader.h"
#include "ComputeShader.h"

ComputeShader::ComputeShader()
{

}

void ComputeShader::Init(ComputeProgram* pProgram, glm::uvec3 group_size, glm::uvec3 element_count)
{
	mpProgram = pProgram;
	mGroupSize = group_size;
	mElementCount = element_count;
}

void ComputeShader::Compute()
{
	glm::uvec3 group_count = {
		(mElementCount.x + mGroupSize.x - 1) / mGroupSize.x,
		(mElementCount.y + mGroupSize.y - 1) / mGroupSize.y,
		(mElementCount.z + mGroupSize.z - 1) / mGroupSize.z
	};
	 
	glDispatchCompute(group_count.x, group_count.y, group_count.z);
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
}

ComputeProgram* ComputeShader::ActivateProgram() 
{
	mpProgram->SetActive();

	for (int i = 0; i < mpBuffers.size(); ++i)
	{
		glBindBufferBase(mpBuffers[i]->GetType(), i, mpBuffers[i]->ssbo);
	}

	return mpProgram;
}

void ComputeShader::SetElementCount(glm::uvec3 element_count)
{
	mElementCount = element_count;
}
