#include <Renderable.h>

namespace X3
{
	Renderable::Renderable()
	{
		mName = "Entity";

		mPosition = glm::vec3(0.f);
		mCenter = glm::vec3(0.f);
		mScale = glm::vec3(1.f);
		mPYR = glm::vec3(0.f);
	}

	void Renderable::SetPass(const RenderPass& pass, const std::string& matName)
	{
		mPasses.insert(std::pair<RenderPass, std::string>(pass, matName));
	}

	std::string Renderable::MatPass(const RenderPass& pass)
	{
		return mPasses.at(pass);
	}

	bool Renderable::HasPass(const RenderPass& pass)
	{
		return mPasses.find(pass) != mPasses.end();
	}

	glm::mat4 Renderable::MatM()
	{
		return  glm::translate(glm::mat4(1.0f), mPosition) *
			glm::scale(glm::mat4(1.0f), mScale) *
			glm::translate(glm::mat4(1.0f), mCenter) *
			glm::mat4_cast(Orientation()) *
			glm::translate(glm::mat4(1.0f), -mCenter);
	}

}